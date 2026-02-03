# Bringup (sem HAT)

Este teste serve para validar a placa da China sem o HAT.
Ele verifica:
- Comunica??o I2C (IMU e Touch aparecem no scan)
- Cart?o SD (init + escrita simples)

## Arduino IDE
1. Abra `examples/bringup/bringup.ino`
2. Selecione a placa ESP32-S3
3. Compile e fa?a upload
4. Abra o Serial Monitor em 115200

## PlatformIO
1. Abra o projeto no PlatformIO
2. Selecione o environment `esp32s3`
3. Fa?a `Build` e `Upload`
4. Abra o Serial Monitor

## Sa?da esperada (exemplo)
- Lista de endere?os I2C
- Mensagem `SD init OK` e `SD write OK`

## Dica
Se o I2C n?o mostrar nenhum dispositivo, verifique os pinos:
- SDA = GPIO47
- SCL = GPIO48

Se o SD falhar, confirme o cart?o e os pinos:
- CS=GPIO38, MOSI=GPIO39, MISO=GPIO40, SCK=GPIO41
