# Mosquitto

## Create a user/password in the pwfile

### login interactively into the mqtt container
sudo docker exec -it <container-id> sh

### Create new password file and add user and it will prompt for password
mosquitto_passwd -c /mosquitto/config/pwfile user1


## Dados Mosquitto
usuario: admin
senha: DEEUFPE
porta: 1884

## topicos
"casa/solar" - recebimento de radiancia
"casa/led"
  on - ligar
  off - desligar
"casa/ventilador"
  horario
  antihorario
  desligar
