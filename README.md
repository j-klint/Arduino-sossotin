# Sossotin
Tällä voi piipittää sosseja ja muita [Morse](https://en.wikipedia.org/wiki/Morse_code)-viestejä, sekä "kansainvälisillä" merkeillä että [katakanalla](https://en.wikipedia.org/wiki/Wabun_code).

Kun nappia painaa, niin vekotin piippaa, tulkkaa painallukset pisteiksi ja viivoiksi, tulkkaa ne kirjoitusmerkeiksi ja kirjoittaa ne LCD-näytölle.

Toimii myös toiseen suuntaan. "Sarjaliitännästä" voi lähettää viestejä Arduinolle piipitettäväksi. Myös joitain parametrejä voi säätää saman liitännän kautta. Jottei tarvitsisi asentaa satojen megatavujen kokoista [Arduino IDE](https://www.arduino.cc/en/software):ä serial monitorin takia, niin kirjoittelin tähän Windowsille kaveriksi pienen command line -ohjelman, jolla myös noita viestejä voi lähetellä.

Kiitokset [David Michalik](https://github.com/dmicha16)lle, joka oli kasannut tarvittavat Windows API:n komennot [yhteen classiin](https://github.com/dmicha16/simple_serial_port).
