const byte temperaturePin = A0; // LM35 sıcaklık sensörü için analog pin ataması
const byte ldrSensorPin = A1; // LDR ışık sensörü için analog pin ataması
const byte triggerPin = 2; // HC-SR04 ultrasonik mesafe sensörünün trigger pini için dijital pin ataması
const byte echoPin = 3; // HC-SR04 ultrasonik mesafe sensörünün echo pini için dijital pin ataması
const byte ldrLedPin = 4; // LDR ışık sensörü ile birlikte çalışacak ışık için dijital pin ataması
const byte pirPin = 5; // PIR sensör için dijital pin ataması
const byte socketPin = 6; // PIR sensörden gelen veriyle kontrol ettiğimiz priz için dijital pin ataması
const byte relayPin = 7; // HC-SR04 ultrasonik mesafe sensörü ile konntrol ettiğimiz motoru çalıştıracak röle için dijital pin ataması

int soundRunTime = 0; // Sesin hoparlörden çıkıp mikrofona ulaşana kadar arada geçen zaman
float distance = 0; // HC-SR04'ün hesaplayacağı mesafe değeri
int temperatureSensorValue = 0; // temperaturePin'den gelen veriyi tutacak değişken
float voltageValue = 0; // temperaturePin'e gelen verinin voltage karşılığını tutacak değiişken
float temperatureValue = 0; // Sıcaklık değerini saklayan değişken
float soundSpeed = 0; //Hesaplanan ses hızını tutan değişken

void setup() // setup() fonksiyonu başlangıçta sadece bir defa çalışır
{
  pinMode(temperaturePin, INPUT); // Sıcaklık sensörü pini INPUT olarak ayarlandı
  pinMode(triggerPin, OUTPUT); // Ultrasonik mesafe sensörünün trigger pini OUTPUT olarak ayarlandı
  pinMode(echoPin, INPUT); // Ultrasonik mesafe sensörünün echo pini INPUT olarak ayarlandı
  pinMode(ldrLedPin, OUTPUT); // LDR sensörü ile kontrol edeceğimiz ledin pini OUTPUT olarak ayarlandı
  pinMode(pirPin, INPUT); // PIR sensörün pini INPUT olarak ayarlandı
  pinMode(socketPin, OUTPUT); // PIR sensörü ile kontrol edeceğimiz prizin pini OUTPUT olarak ayarlandı
  pinMode(relayPin, OUTPUT); // Ultrasonik mesafe sensörü aracılığıyla kontrol edeceğimiz motoru aktifleştiren rölenin pini OUTPUT olarak ayarlandı

  Serial.begin(9600); // Seri iletişim başlatıldı. Saniye başına gönderilen ve alınan bit sayısı: 9600
}

void loop() // loop() fonksiyonu, setup() fonksiyonundan sonra çalışır ve sonsuz döngü şeklinde tekrar başa dönerek kendini tekrar eder
{
  // Sıcaklık sensöründen gelen veriyi Celcius'a dönüştürme (LM35 sensör için)
  temperatureSensorValue = analogRead(temperaturePin); // Sıcaklık sensöründen gelen veriyi oku ve temperatureSensorValue değişkenine ata
  // Sıcaklık sensörü sıcaklığa bağlı olarak 0-1023 arasında değerler üretir. Bu değerler 0V-5V arasındaki gerilim değerleri olarak Arduino'ya iletilir
  voltageValue = (temperatureSensorValue / 1023.0) * 5000.0; // milivolt cinsinden voltaj değerinin hesaplanması. 5000 değeri 5V = 5000mV değerine karşılık gelir
  temperatureValue = voltageValue / 10.0; // Her 10 mV gerilime karşılık değer 1 artar. Elde edilen gerilim 10'a bölündüğünde sıcaklık değeri elde edilir

  // Celsius cinsinden sıcaklık değerini serial monitore yazdır
  Serial.print("Temperature: ");
  Serial.print(temperatureValue);
  Serial.println(" Celsius");

  digitalWrite(triggerPin, LOW); // başlangıçta trigger pininin aktif olmadığına emin olmak için LOW yapıldı
  delayMicroseconds(2); // 2 microseconds gecikme
  digitalWrite(triggerPin, HIGH); // trigger pini aktifleştirildi
  delayMicroseconds(1000); // 1000 microseconds süre boyunca trigger pini aktif kalır
  digitalWrite(triggerPin, LOW); // trigger pinine verilen güç kesildi
  soundRunTime = pulseIn(echoPin, HIGH); // echo pinine sinyal gelene kadar echoPin dinlenir. Geçen süre değişkene atanır

  soundSpeed = 331.3 * sqrt(1 + (temperatureValue / 273.0)); // Celsius cinsinden sıcaklığa bağlı olarak ses hızının m/s olarak hesaplanması. Ses hızı formülünden yararlanıldı

  // m/s cinsinden ses hızını serial monitore yazdırma
  Serial.print("Sound Speed: ");
  Serial.print(soundSpeed);
  Serial.println(" m/s");

  soundSpeed = (soundSpeed * 100) / 1000000; // m/s olan birimi cm/us (microseconds) birimine dönüştürme

  distance = (soundRunTime / 2.0) * soundSpeed; // Ses, sensöre geri dönene kadar aynı yolu iki defa alır. Bu sebeple geçen zamanın yarısı ile ses hızı çarpılarak mesafe değeri elde edilir

  // cm cinsinden mesafe değerini serial monitore yazdırma
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // HC-SR04 ultrasonik mesafe sensöründen gelen değer ile hesaplanan mesafe değeri baz alınarak, DC motoru kontrol eden röleyi aktifleştirme
  if (distance >= 5 && distance <= 8.5) { // Sensörün yerleştirildiği konuma bağlı olarak belli aralıkta çalışır
    digitalWrite(relayPin, LOW); // DC motorun bulunduğu devrenin röleye bağlanma şekline bağlı olarak LOW değeri verildiğinde motor çalışır
  }
  else {
    digitalWrite(relayPin, HIGH); // DC motorun bulunduğu devrenin röleye bağlanma şekline bağlı olarak LOW değeri verildiğinde motor durur
  }
  // LDR sensor ile odanın ışığını ayarlama
  int ldrSensorValue = analogRead(A1); // Ortamın ışığına bağlı olarak LDR'nin ürettiği 0-1023 arasındaki değer bir değişkene atanır
  int ldrThreshold = 300; // Oda aydınlatmasını çalıştırmak için baz aldığımız ortamın ışık değeri, sınır değeri

  // LDR sensörün değerini serial monitore yazdır
  Serial.print("ldr value: ");
  Serial.println(ldrSensorValue);

  if (ldrSensorValue <= ldrThreshold) { // Sesörden okunan değer, sınır değerine eşit veya küçük olup olmama durumu
    digitalWrite(ldrLedPin, HIGH); // Koşul doğru ise aydınlatma çalışır
  }
  else {
    digitalWrite(ldrLedPin, LOW); // Koşul doğru değilse aydınlatma çalışmaz
  }

  // PIR sensör ile prizin elektriğinin kontrolü
  if (digitalRead(pirPin) == LOW) { // PIR sensörü sinyal göndermiyorsa
    digitalWrite(socketPin, HIGH); // PIR sensörü sinyal göndermiyorsa, prizin elektriği aktifleştirilir
  }
  else {
    digitalWrite(socketPin, LOW); // PIR sensörü sinyal gönderiyorsa, prizin elektriği kesilir
  }

  Serial.println(); // serial monitorde değerlerin daha rahat okunması amacıyla her iterasyonda bir satır boşluk bırakır
  delay(200); // 200 milisaniye gecikme
}
