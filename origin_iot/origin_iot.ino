// #include "DHT.h"
#include <DHT20.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include <HTTPClient.h>
#include <ArduinoJson.h> //Arduino Json by Benoit Blanchon



#define sensorPin 32
DHT20 DHT(&Wire1); 

// 接続先のSSIDとパスワード
const char* ssid = ""; //無線ルーターのssidを入力
// const char* ssid = "";
const char* password = ""; //無線ルーターのパスワードを入力


char* message = "病害・害虫発生警告";

const char* host = "notify-api.line.me";
const char* token = ""; //Your API key

const String endpoint = "http://api.openweathermap.org/data/2.5/forecast?q=kofu&units=metric&lang=ja&APPID=";
const String key ="";

void setup() {
  Serial.begin(115200);
  Serial.println("DHT20 test!");
  Wire1.begin(33, 32);
  pinMode(sensorPin, INPUT);

}

void connectWiFi(){

  Serial.print("ssid:");
  Serial.print(ssid);
  Serial.println(" に接続します。");

  WiFi.begin(ssid, password);

  Serial.print("WiFiに接続中");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("接続しました。");

  //IPアドレスの表示
  Serial.print("IPアドレス:");
  Serial.println(WiFi.localIP());

}

int count0 = 0;
void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  DHT.read(); 
  float humidity = DHT.getHumidity();          // 湿度の読み取り
  float temperature = DHT.getTemperature(); 

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Print the humidity and temperature
  Serial.print("Humidity: "); 
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(temperature);
  Serial.println(" *C");

  float avghumi = sumHumi(humidity); //sumhumiに引数
  float avgtem = sumTemp(temperature); //sumtempに引数
//一日に一回スプレッドシートに書き込み
  count0++;
  if (count0 == 24){
    float sumgra = grapesumT(avgtem); //平均気温をブドウの有効積算へ　

    WiFiClientSecure sslclient; //httpsに送るため

  const char* server = "script.google.com";
  String url = "https://script.google.com/macros/s/AKfycbzIxPSa6oZDKVqeir8n3mNzhEFWVVZUBcxSFYrG6JQdEqJQeFM2vqY8EQqXT1QW1mZKgA/exec";  //googlescript web appのurlを入力

    //ここでスプレッドシート送信の変数宣言　気温・湿度・積算
    float sensor_data1= avghumi; //平均気温
    float sensor_data2= avgtem; //平均湿度
    float sensor_data3= sumgra; //積算

    //wifiに接続
  connectWiFi();

  //測定値の表示
  Serial.println(sensor_data1);
  Serial.println(sensor_data2);
  Serial.println(sensor_data3);
     //urlの末尾に測定値を加筆  これらは自分のほうのプログラムに書く
    url += "?";
    url += "&1_cell=";
    url += sensor_data1;
    url += "&2_cell=";
    url += sensor_data2;
    url += "&3_cell=";
    url += sensor_data3;

    // サーバーにアクセス
    Serial.println("サーバーに接続中...");
    sslclient.setInsecure();//skip verification
  
    //データの送信
    if (!sslclient.connect(server, 443)) {
      Serial.println("接続に失敗しました");
      Serial.println("");//改行
      return;
    }

    Serial.println("サーバーに接続しました");

    sslclient.println("GET " + url);//クライアントに送る
    delay(1000); //私の環境ではここに待ち時間を入れないとデータが送れないことがある
    sslclient.stop();

    Serial.println("データ送信完了");
    Serial.println("");//改行

    //WiFiを切断
    WiFi.mode(WIFI_OFF);
    
    openwh();

    count0 = 0;
  }
  delay(1000);
  //delay(3600000);

}

float sum1 = 0;
int count1 = 0;
float sumHumi(float h){
    sum1 += h;
    count1++; 
  if(count1 == 24){
   float sum1h = sum1/24.0;
    sum1 = 0;
    count1 = 0;
    return sum1h;//返り値平均湿度を渡す
  } else {
    return 0;
  }
}

float sum2 = 0;
int count2 = 0;
float sumTemp(float t){
    sum2 += t;
    count2++;
  if(count2 == 24){
    float sum2t = sum2/24.0;
    buibuiAlart(sum2t);//以下各害虫の関数へ
    kuwakonaAlart(sum2t);
    yotouAlart(sum2t);
 
    sum2 = 0;
    count2 = 0;
    return sum2t;//返り値平均気温を渡す 
  } else {
    return 0;
  }
}
//有効積算温度についてはブドウはいつでも、害虫は3月あたりからが好ましい（1月からでも）
float sumt0 = 0;
float grapesumT(double tt){ //ブドウの積算温度　10℃以上を―10してカウント
  if (tt > 10){
    sumt0 += tt - 10; //どんどん足されていく
    return sumt0; //返り値積算渡す
  } else {
    return sumt0;
  }
}

float sumt1 = 0;
int flag1 = 0;
void buibuiAlart(float t1){ //虫の発生する積算温度と照らし合わせる 生育ステージごと条件
  if (t1 > 14.9 && sumt1 < 121.7){
    sumt1 += t1 - 14.9;
  } else if (t1 > 15.0 && 121.7 <= sumt1 && sumt1 < 289.8){
    sumt1 += t1 - 15.0;
  } else if (t1 > 13.0 && 289.8 <= sumt1 && sumt1 < 489.8){
    sumt1 += t1 - 13.0;
  } else if (t1 > 7.5 && 489.8 <= sumt1 && sumt1 < 1400.5){
    sumt1 += t1 - 7.5;
  } else if (t1 > 13.9 && 1400.5 <= sumt1 && sumt1 < 1459){
    sumt1 += t1 - 13.9;
  } else if (t1 > 14.2 && 1459 <= sumt1 && sumt1 < 1612.1){
    sumt1 += t1 - 14.2;
  }

//予報と組み合わせて発生予測1週間後

    if (flag1 == 0){
    if (sumt1 >= 1612.1){
      //ドウガネブイブイ成虫発生警告をLINE
      // char* message = "ドウガネブイブイ成虫発生の可能性";
      send_line();
      Serial.println("ドウガネブイブイ成虫");
      flag1 = 1;
    }
  }
}



float sumt2 = 0;
int flag2 = 0;
void kuwakonaAlart(float t2){ //幼虫発生時期　
  if (t2 > 12.3 && sumt2 < 127){
    sumt2 += t2 - 12.3;
  }
  
 if (flag2 == 0){
   if (sumt2 >= 127){
    //クワコナ幼虫発生警告をLINE
    // char* message = "クワコナカイガラムシ幼虫発生の可能性";
    send_line();
    Serial.println("クワコナ幼虫");
     flag2 = 1;
    }    
  }
}



float sumt3 = 0;
int flag3 = 0;
void yotouAlart(float t3){ //幼虫発生時期　
  if (t3 > 8.6 && sumt3 < 58.3){
    sumt3 += t3 - 8.6;
  }
  
 if (flag3 == 0){
   if (sumt3 >= 58.3){
    //ヨトウガ幼虫発生警告をLINE
    send_line();
    Serial.println("ヨトウガ幼虫");
     flag3 = 1;
    }    
  }
}

void send_line() {
  connectWiFi();
  //Access to HTTPS (SSL communication)
  WiFiClientSecure client;
  //Required when connecting without verifying the server certificate
  client.setInsecure();//sa-ba-karahajikarerumonowo toosu 

  Serial.println("Try");

  //SSL connection to Line's API server (port 443: https)
  if (!client.connect(host, 443)) {
    Serial.println("Connection failed");
    return;
  }
  Serial.println("Connected");

  // Send request
  String query = String("message=") + String(message);
  String request = String("") +
    "POST /api/notify HTTP/1.1\r\n" +
    "Host: " + host + "\r\n" +
    "Authorization: Bearer " + token + "\r\n" +
    "Content-Length: " + String(query.length()) +  "\r\n" + 
    "Content-Type: application/x-www-form-urlencoded\r\n\r\n" +
    query + "\r\n";
  client.print(request);
 
  // Wait until reception is complete
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }
  
  String line = client.readStringUntil('\n');
  Serial.println(line); //1dが返ってくる　うまくいくと

  WiFi.mode(WIFI_OFF);
}

void openwh(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
  if ((WiFi.status() == WL_CONNECTED)) {
 
    HTTPClient http;
 
    http.begin(endpoint + key); //URLを指定
    int httpCode = http.GET();  //GETリクエストを送信
 
    if (httpCode > 0) { //返答がある場合
 
        String payload = http.getString();  //返答（JSON形式）を取得
        //Serial.println(httpCode);
        //Serial.println(payload);

        //jsonオブジェクトの作成
        DynamicJsonDocument forecaseDoc(1024);
        String json = payload;
        deserializeJson(forecaseDoc, json);

        //各データを抜き出し日程は"list"で指定

        float arh3[8] = {0};//20-27
        float arh4[8] = {0};//28-35
        float arh5[5] = {0};//5日後については15時間分 36-40
        
        float sumh3 = 0;
        float sumh4 = 0;
        float sumh5 = 0;
        for (int i = 20; i < 28; i++){
          arh3[i - 20] = forecaseDoc["list"][i]["main"]["humidity"];
          sumh3 += arh3[i - 20];
        }
        for (int i = 28; i < 36; i++){
          arh4[i - 28] = forecaseDoc["list"][i]["main"]["humidity"];
          sumh4 += arh4[i - 28];
        }
        for (int i = 36; i < 41; i++){
          arh5[i - 36] = forecaseDoc["list"][i]["main"]["humidity"];
          sumh5 += arh5[i - 36];
        }
       
       float avgh3 = sumh3 / 8;
       float avgh4 = sumh4 / 8;
       float avgh5 = sumh5 / 5;

        // double temp = forecaseDoc["main"]["temp"];
        
        //Serial.printf("wheatherId:%d discription:%s rain:%.2f snow:%.2f\n", wheatherId, discription.c_str(), rain, snow);
        Serial.printf("3day:%.2f\n", avgh3);
        Serial.printf("4day:%.2f\n", avgh4);
        Serial.printf("5day:%.2f\n", avgh5);


        if (70 <= avgh3 && 70 <= avgh4 && 70 <= avgh5){
          send_line();
          Serial.printf("高湿度警告");
        }

        // Serial.printf("main:%.2f\n", temp);
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
  WiFi.mode(WIFI_OFF);
}