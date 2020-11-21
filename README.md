# AnxietyStation

Estación diseñada para predecir tus niveles de ansiedad dependiendo de mediciones ambientales y de salud. Tomamos en cuenta la cantidad de luz y la calidad del aire del lugar donde te encuentras y medimos tus pulsaciones para obtener la concentración de oxígeno en sangre.

## Lista de componentes utilizados

- Placa arduino mega
- Sensor de calidad de aire y concentración de gases MQ135 de AZ delivery
- Sensor de luz MH Sensor Series
- Sensor de pulsaciones y oxigenación MAX30102 MH ETLive
- Pantalla 1602 Display/1602 Screen con potenciómetro
- Botón de encendido
- Led azul de encendido
- Leds de colores por cada sensor: verde, amarillo y rojo

## Pasos seguidos

- Diseñar el circuito (adjunto en el repositorio)
- [Documentación para codificar la parte de sensor de calidad de aire MQ135](https://www.luisllamas.es/arduino-detector-gas-mq/)
- Para usar la pantalla LCD utilizamos la librería [LiquidCrystal](https://www.arduino.cc/en/Reference/LiquidCrystal) y probamos varias aproximaciones, como tener una línea estática y otra en movimiento, pero acabamos dejándolo con las dos estáticas pero con los valores cambiando.
- [Documentación en la que nos basamos para la parte del sensor de luz](https://www.luisllamas.es/medir-cantidad-de-luxes-con-arduino-y-el-luxometro-bh1750/)
- [Libreria Sparkfun usada paa el sensor de pulsaciones y oxigenación MAX30102](https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library)

## Observaciones
Mientras que los valores de luz y calidad de aire parecen bastante precisos, los de oxigenación no nos gustan mucho, porque vemos que varían y dan mediciones que vemos no muy precisas, pero por lo que hemos leído todos los sensores funcionan de forma similar.


----------------------------------

# AnxietyStation

Station designed to predict your anxiety levels depending on environmental and health measurements. We take into account the amount of light and the quality of the place´s air where you are. We also measure your pulsations, to obtain the oxygen´s concentration in blood.

## List of used components

- Arduino mega board
- Air quality and gas concentration ssensor MQ135 from AZ delivery
- Light Sensor MH Sensor Series
- Pulsation and oxygenation sensor MAX30102 MH ETLive
- 1602 Display / 1602 Screen with potentiometer
- Power button
- Blue power LED
- Colored LEDs for each sensor: green, yellow and red

## Steps followed

- Design the circuit (attached in the repository)
- [Documentation for coding the MQ135 air quality sensor part](https://www.luisllamas.es/arduino-detector-gas-mq/)
- To use the LCD screen we used the LiquidCrystal library and we tried various approaches, such as having a static line and a moving line, but we ended up leaving it with both static but with the values changing.
- [Documentation on which we rely for the light sensor part](https://www.luisllamas.es/medir-cantidad-de-luxes-con-arduino-y-el-luxometro-bh1750/)
- [Sparkfun library used for the MAX30102 heart rate and oxygenation sensor](https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library)

## Observations
While the light and air quality values seem quite accurate, we don't think the oxygenation values were precise. We concluded this due to the fact that they vary and give diverse measurements while measuring the same sample. But from what we have read, all the pulse sensors available work in a similar way.


