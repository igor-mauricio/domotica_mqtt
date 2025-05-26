package main

import (
	"bufio"
	"encoding/json"
	"fmt"
	"io"
	"log"
	"os"
	"path/filepath"
	"strings"

	mqtt "github.com/eclipse/paho.mqtt.golang"
	"github.com/tarm/serial"
)

// Config represents the structure of the JSON config file
type Config struct {
	MQTTBroker   string   `json:"mqtt_broker"`
	MQTTUser     string   `json:"mqtt_user"`
	MQTTPassword string   `json:"mqtt_password"`
	Topics       []string `json:"topics"`
	SerialPort   string   `json:"serial_port"`
	BaudRate     int      `json:"baud_rate"`
}

func main() {
	// Load configuration
	config, err := loadConfig("config.json")
	if err != nil {
		log.Fatalf("Failed to load config: %v", err)
	}

	// If SerialPort is not specified, search for USB devices
	if config.SerialPort == "" {
		config.SerialPort, err = findUSBDevice()
		if err != nil {
			log.Fatalf("Failed to find USB device: %v", err)
		}
		log.Printf("Using detected serial port: %s", config.SerialPort)
	}

	// Open serial connection
	serialConfig := &serial.Config{Name: config.SerialPort, Baud: config.BaudRate}
	serialPort, err := serial.OpenPort(serialConfig)
	if err != nil {
		log.Fatalf("Failed to open serial port: %v", err)
	}
	defer serialPort.Close()

	// Connect to MQTT broker
	opts := mqtt.NewClientOptions().AddBroker(config.MQTTBroker)
	if config.MQTTUser != "" && config.MQTTPassword != "" {
		opts.SetUsername(config.MQTTUser)
		opts.SetPassword(config.MQTTPassword)
	}
	client := mqtt.NewClient(opts)
	if token := client.Connect(); token.Wait() && token.Error() != nil {
		log.Fatalf("Failed to connect to MQTT broker: %v", token.Error())
	}
	defer client.Disconnect(250)

	// Subscribe to topics
	for _, topic := range config.Topics {
		topic := topic // capture range variable
		client.Subscribe(topic, 0, func(client mqtt.Client, msg mqtt.Message) {
			// Send received MQTT message to Arduino via serial
			message := fmt.Sprintf("%s:%s\n", msg.Topic(), string(msg.Payload()))
			_, err := serialPort.Write([]byte(message))
			print("Sending to serial: ", message)
			if err != nil {
				log.Printf("Failed to write to serial port: %v", err)
			}
		})
	}

	// Read from serial and publish to MQTT
	reader := bufio.NewReader(serialPort)
	for {
		// Read until newline
		data, err := reader.ReadString('\n')
		if err != nil {
			log.Printf("Failed to read from serial port: %v", err)
			continue
		}
		data = strings.TrimSpace(data) // Remove any trailing newline or whitespace

		// Parse topic and message from serial data
		parts := strings.SplitN(data, ":", 2) // Split into two parts: topic and message
		if len(parts) != 2 {
			log.Printf("Failed to parse serial data: invalid format [%s]", data)
			continue
		}
		topic := parts[0]
		message := parts[1]

		println("Received from serial: [", data, "]")
		println("Parsed topic: ", topic, " message: ", message)

		// Publish to MQTT
		client.Publish(topic, 0, false, message)
	}
}

// loadConfig reads and parses the JSON configuration file
func loadConfig(filename string) (*Config, error) {
	file, err := os.Open(filename)
	if err != nil {
		return nil, err
	}
	defer file.Close()

	data, err := io.ReadAll(file)
	if err != nil {
		return nil, err
	}

	var config Config
	if err := json.Unmarshal(data, &config); err != nil {
		return nil, err
	}
	return &config, nil
}

// findUSBDevice searches for available USB serial devices
func findUSBDevice() (string, error) {
	// Look for devices in /dev/ matching tty.usb*
	devices, err := filepath.Glob("/dev/tty.usbserial-0001")
	if err != nil {
		return "", fmt.Errorf("failed to search for USB devices: %v", err)
	}

	if len(devices) == 0 {
		return "", fmt.Errorf("no USB devices found")
	}

	// Return the first available device
	return devices[0], nil
}
