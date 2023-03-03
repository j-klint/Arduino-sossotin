# Sossotin
Tällä voi piipittää sosseja ja muita [Morse](https://en.wikipedia.org/wiki/Morse_code)-viestejä niin "kansainvälisillä" merkeillä kuin [katakanallakin](https://en.wikipedia.org/wiki/Wabun_code).

Kun nappia painelee, niin vekotin piippaa, tulkkaa painallukset pisteiksi ja viivoiksi, tulkkaa ne kirjoitusmerkeiksi ja kirjoittaa ne LCD-näytölle. Muita [prosignejä](https://en.wikipedia.org/wiki/Prosigns_for_Morse_code) en vaivautunut ohjelmoimaan kuin nuo **\<DO\>** ja **\<SN\>**, joilla vaihdetaan japaniin ja takaisin.

Toimii myös toiseen suuntaan. "Sarjaliitännästä" voi lähettää viestejä Arduinolle piipitettäväksi. Myös joitain parametrejä voi säätää saman liitännän kautta. Jottei tarvitsisi asentaa satojen megatavujen kokoista [Arduino IDE](https://www.arduino.cc/en/software):ä pelkän serial monitorin takia, niin kirjoittelin tähän Windowsille kaveriksi pienen command line -ohjelman, jolla myös noita viestejä voi lähetellä.

Kiitokset [David Michalik](https://github.com/dmicha16)ille, joka oli kasannut tarvittavat Windows API:n komennot [yhteen classiin](https://github.com/dmicha16/simple_serial_port).


## Ohjeita
Arduino-koodi on tehty ja pitäisi kääntyä ongelmitta Arduino IDE:llä, joka sen sitten toivon mukaan osaa lähettää Arduinolle.

Windows-koodi on ihan perus Visual Studio -projekti/solution ja pitäisi toimia Visual Studiossa build-nappia painamalla. Muistakaa kääntää Release-modessa, ellette halua tarpeettomia debugausviestejä (Makro NDEBUG pitää olla määriteltynä).

Ohjelmaa ajaessa se parametrien lähettäminen PC:ltä Arduino-ohjelmalle toimii niin, että pitää kirjoittaa lukuarvo heti siihen komennon perään ilman välilyöntiä.

Seuraavasta kuvasta pitäisi kytkentöjen käydä ilmi. LCD-näytön tulee tietysti olla sellainen, että Arduinon [\<LiquidCrystal.h\>](https://www.arduino.cc/reference/en/libraries/liquidcrystal/)-kirjasto sitä osaa käyttää jne.
![kytkentäkaavio](/pics/kaavio.png)

## Esimerkki
Pääsin räpeltämään Kemin Pohjantähti-opiston laitteilla ja tekaisin itse melulaatikon. Kuulokepiuhalla kytketyn morsetusavaimen malli löytyi [Thingiversestä](https://www.thingiverse.com/thing:2870754) ja se 3D-printattiin. Muutaman esittelykuvan laitoin [pics-hakemistoon](/pics) (Ovat isoja!). Jos jaksan, niin ehkä uploadaan videonpätkänkin joskus.
