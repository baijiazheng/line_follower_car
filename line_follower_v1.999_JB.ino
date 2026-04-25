//=================================1.硬件定义层==================================全大写
//左边两个舵机
#define MOTOR_LEFT_IN1 12
#define MOTOR_LEFT_IN2 13
#define MOTOR_LEFT_PWM 11
//右边两个舵机
#define MOTOR_RIGHT_IN1 8
#define MOTOR_RIGHT_IN2 9
#define MOTOR_RIGHT_PWM 10
//红外循迹模块
#define IR_LEFTMOST     A0
#define IR_LEFT         A1
#define IR_CENTER       A2
#define IR_RIGHT        A3
#define IR_RIGHTMOST    A4
//=================================2.可调参数区==================================全大写
//小车的基本循迹速度参数
const int   SPEED_BASE                     = 131;
const int   PWM_MAX                        = 150;
//PID算法的参数
const float PID_KP_DEFAULT                 = 10.0;
//关于转动幅度的全部参数
const float TURN_GAIN_LOW                  = 25;
const float TURN_GAIN_HIGH                 = 55;
//非阻塞所需变量
const unsigned long SENSOR_READ_INTERVAL   = 10;
//直线死区
const float TURN_DEADBAND                  = 0.4;
//=================================3.全局状态区==================================小驼峰
//红外循迹的信息存储
bool isBlack_LeftMost                = false;
bool isBlack_Left                    = false;
bool isBlack_Center                  = false;
bool isBlack_Right                   = false;
bool isBlack_RightMost               = false;
//非阻塞所需变量
unsigned long nextSensorReadTime     = 0;
//PID用到的变量
float error;
float lastError;
//存放蓝牙模式的参数
int lastJoyCmdTime;
const int JOY_TIMEOUT                = 200;//采用心跳检测
char currentDriveCmd                 = ' '; // 当前的运动指令，默认为空转/停止
int  currentDriveSpeed               = 100; // 当前的速度设定值
//有关mode的参数
bool onOffAutoState                  = 1;//0为自动循迹，1为蓝牙控制
String btCommand                     = ""; // 存储一条完整蓝牙指令
//=================================4.函数声明区==================================大驼峰
void MotorPolarityTest();
void MotorStop();
void SensorRead();
float ErrorCalculate();
float ComputeTurn(float err);
void MotorDrive(float turn);
void JOY_Control ();
void manualDriveUpdate();
void JOY_SixDirection (char myCmd, int speedVal);
void JOY_ChangeSpeed (char myCmd);
void handleBluetoothCommand();
//=================================5. 初始化  ===================================
void setup() {
  //清理内存
  Serial.begin(9600);
  // 清空串口缓冲区，丢弃上电瞬间的干扰数据
  while (Serial.available()) {
      Serial.read();
  }
  // 延迟一小段时间，给串口一个稳定的起始点
  delay(10); 
  Serial.println("System Ready!");
  // 对电机驱动模块和红外循迹模块定义
  pinMode(MOTOR_LEFT_IN1,OUTPUT);
  pinMode(MOTOR_LEFT_IN2,OUTPUT);
  pinMode(MOTOR_LEFT_PWM,OUTPUT);

  pinMode(MOTOR_RIGHT_IN1,OUTPUT);
  pinMode(MOTOR_RIGHT_IN2,OUTPUT);
  pinMode(MOTOR_RIGHT_PWM,OUTPUT);

  pinMode(IR_LEFTMOST,INPUT);
  pinMode(IR_LEFT,INPUT);
  pinMode(IR_CENTER,INPUT);
  pinMode(IR_RIGHT,INPUT);
  pinMode(IR_RIGHTMOST,INPUT);
  
  //MotorPolarityTest();
  //测试电机极性，不需要时注释
  Serial.println("===========line_follower===========");
  Serial.println("");
  Serial.println("===================================");
  Serial.print("SPEED_BASE"); Serial.println(SPEED_BASE);
  Serial.print("PID_KP_DEFAULT"); Serial.println(PID_KP_DEFAULT);
  Serial.println("===================================");
  Serial.println("");
}
//=================================6. 主循环  ===================================
void loop() {
  SensorRead();          // 每 10ms 更新一次传感器状态
  // 调试：每 100ms 打印一次偏差，不需要时注释
  // static unsigned long lastPrintTime = 0;
  // unsigned long now = millis();
  // if (now - lastPrintTime >= 100) {
  //     lastPrintTime = now;
  //     float err = ErrorCalculate();
  //     Serial.print("Error: ");
  //     Serial.println(err);
  // }

  //识别是否有串口信息
  if(Serial.available()){
    char c = Serial.peek();//先看再取防止丢数据
    if(c == 'm'){
      btCommand = Serial.readStringUntil('\n');
      btCommand.trim(); // 去除 '\r' 和空格
      onOffAutoState = !onOffAutoState;
      Serial.println ("====模式已切换====");
      Serial.println (onOffAutoState?"JOY":"AUTO");
      motorStop(); // 切换模式必须停车！
      //清理串口
      while(Serial.available()) Serial.read();
    }else if (onOffAutoState){
      handleBluetoothCommand();
    }
  }

  static unsigned long lastPidTime = 0;
  unsigned long now = millis();
  if (onOffAutoState ==0 && now >= lastPidTime + 20){
    lastPidTime = now;

    float err= ErrorCalculate();
    float turn = ComputeTurn(err);
    MotorDrive(turn);
  }
  else {
    manualDriveUpdate();
  }
}
//=================================7.功能实现层==================================
void MotorPolarityTest() {
  // 测试左轮：左轮正转，右轮刹车
  digitalWrite(MOTOR_LEFT_IN1, HIGH);
  digitalWrite(MOTOR_LEFT_IN2, LOW);
  analogWrite(MOTOR_LEFT_PWM, 80);

  digitalWrite(MOTOR_RIGHT_IN1, HIGH);
  digitalWrite(MOTOR_RIGHT_IN2, HIGH);
  analogWrite(MOTOR_RIGHT_PWM, 0);

  delay(2000);

  // 停止
  digitalWrite(MOTOR_LEFT_IN1, HIGH);
  digitalWrite(MOTOR_LEFT_IN2, HIGH);
  analogWrite(MOTOR_LEFT_PWM, 0);
  delay(500);

  // 测试右轮：右轮正转，左轮刹车
  digitalWrite(MOTOR_RIGHT_IN1, HIGH);
  digitalWrite(MOTOR_RIGHT_IN2, LOW);
  analogWrite(MOTOR_RIGHT_PWM, 80);

  digitalWrite(MOTOR_LEFT_IN1, HIGH);
  digitalWrite(MOTOR_LEFT_IN2, HIGH);
  analogWrite(MOTOR_LEFT_PWM, 0);

  delay(2000);

  // 停止
  digitalWrite(MOTOR_RIGHT_IN1, HIGH);
  digitalWrite(MOTOR_RIGHT_IN2, HIGH);
  analogWrite(MOTOR_RIGHT_PWM, 0);
}

void motorStop(){
  digitalWrite(MOTOR_RIGHT_IN1, HIGH);
  digitalWrite(MOTOR_RIGHT_IN2, HIGH);
  analogWrite(MOTOR_RIGHT_PWM, 0);
  digitalWrite(MOTOR_LEFT_IN1, HIGH);
  digitalWrite(MOTOR_LEFT_IN2, HIGH);
  analogWrite(MOTOR_LEFT_PWM, 0);
}

void SensorRead(){
  unsigned long now = millis();
  if (now >= nextSensorReadTime) {
    nextSensorReadTime = now + SENSOR_READ_INTERVAL; //直接记录下一次的时间，避免累计误差
    //读信息
    isBlack_LeftMost  = (digitalRead(IR_LEFTMOST)  == LOW);
    isBlack_Left      = (digitalRead(IR_LEFT)      == LOW);
    isBlack_Center    = (digitalRead(IR_CENTER)    == LOW);
    isBlack_Right     = (digitalRead(IR_RIGHT)     == LOW);
  }
}

float ErrorCalculate(){
  int sum   = 0;
  int count = 0;
  if(isBlack_LeftMost)  {sum -=3 ; ++count;}
  if(isBlack_Left)      {sum -=1 ; ++count;}
  if(isBlack_Center)    {sum +=1 ; ++count;}
  if(isBlack_Right)     {sum +=3 ; ++count;}

  if (count == 0) return 0;
  if (count == 4) return lastError;

  float currentError = (float)sum / count;
  lastError = currentError;//更新error
  return currentError;
}

float ComputeTurn(float err) {
  float turn = PID_KP_DEFAULT * err;

  if (turn > TURN_GAIN_HIGH) turn = TURN_GAIN_HIGH;
  if (turn < -TURN_GAIN_HIGH) turn = -TURN_GAIN_HIGH;

  return turn;
}

void MotorDrive(float turn) {
  int leftSpeed , rightSpeed ;
  //判断要不要无敌强力转动
  if (turn > TURN_DEADBAND){
    leftSpeed = SPEED_BASE;
    rightSpeed= -SPEED_BASE;
  }
  else if (turn < -TURN_DEADBAND){
    leftSpeed =-SPEED_BASE;
    rightSpeed= SPEED_BASE;
  }
  else{//差不多是直线了，按之前的判断调速
    leftSpeed = SPEED_BASE - turn;
    int rightBias = 8;
    rightSpeed= SPEED_BASE + turn +rightBias;
  }


  leftSpeed = constrain(leftSpeed,-PWM_MAX,PWM_MAX);
  rightSpeed= constrain(rightSpeed,-PWM_MAX,PWM_MAX);

  // 根据速度符号设置电机方向
  // 左电机
  if (leftSpeed>=0){
  digitalWrite(MOTOR_LEFT_IN1,HIGH);
  digitalWrite(MOTOR_LEFT_IN2,LOW);
  analogWrite(MOTOR_LEFT_PWM,leftSpeed);
  }else{
  digitalWrite(MOTOR_LEFT_IN1,LOW);
  digitalWrite(MOTOR_LEFT_IN2,HIGH);
  analogWrite(MOTOR_LEFT_PWM,-leftSpeed);
  }
  //右电机 
  if (rightSpeed>=0){
  digitalWrite(MOTOR_RIGHT_IN1,HIGH);
  digitalWrite(MOTOR_RIGHT_IN2,LOW);
  analogWrite(MOTOR_RIGHT_PWM,rightSpeed);
  }else{
  digitalWrite(MOTOR_RIGHT_IN1,LOW);
  digitalWrite(MOTOR_RIGHT_IN2,HIGH);
  analogWrite(MOTOR_RIGHT_PWM,-rightSpeed);
  }
  // Serial.print("Turn: "); Serial.print(turn);
  // Serial.print(" L: "); Serial.print(leftSpeed);
  // Serial.print(" R: "); Serial.println(rightSpeed);
}

// void JOY_Control (){//旧版本
//   if (Serial.available()){
//     char cmd=Serial.read();
//     // JOY_SixDirection(cmd);
//     JOY_ChangeSpeed(cmd);
//     if (cmd=='w'||cmd=='s'||cmd=='a'||cmd=='d'||cmd==' '||cmd=='x'){
//       lastJoyCmdTime = millis();
//     }
//   }
//   //超时停车
//   if (millis()- lastJoyCmdTime >= JOY_TIMEOUT) motorStop();
// }

void manualDriveUpdate() {
  // 检查心跳是否超时
  if (millis() - lastJoyCmdTime >= JOY_TIMEOUT) {
    motorStop(); // 超时停车
    currentDriveCmd = ' '; // 指令重置为空转
    return; // 停车后，本次更新结束
  }

  // 如果没超时，就根据 currentDriveCmd 执行对应的动作
  JOY_SixDirection(currentDriveCmd, currentDriveSpeed); 
}

void JOY_SixDirection (char myCmd, int speedVal){
  if (myCmd=='w'||myCmd=='s'||myCmd=='a'||myCmd=='d'||myCmd==' '||myCmd=='x'){
    switch (myCmd){
      case 'w'://前进
      digitalWrite (MOTOR_LEFT_IN1,HIGH);
      digitalWrite (MOTOR_LEFT_IN2,LOW);
      analogWrite (MOTOR_LEFT_PWM,speedVal);
      digitalWrite (MOTOR_RIGHT_IN1,HIGH);
      digitalWrite (MOTOR_RIGHT_IN2,LOW);
      analogWrite (MOTOR_RIGHT_PWM,speedVal);
      break;

      case 's'://后退
      digitalWrite (MOTOR_LEFT_IN1,LOW);
      digitalWrite (MOTOR_LEFT_IN2,HIGH);
      analogWrite (MOTOR_LEFT_PWM,speedVal);
      digitalWrite (MOTOR_RIGHT_IN1,LOW);
      digitalWrite (MOTOR_RIGHT_IN2,HIGH);
      analogWrite (MOTOR_RIGHT_PWM,speedVal);
      break;

      case 'a'://左转
      digitalWrite (MOTOR_LEFT_IN1,LOW);
      digitalWrite (MOTOR_LEFT_IN2,HIGH);
      analogWrite (MOTOR_LEFT_PWM,speedVal);
      digitalWrite (MOTOR_RIGHT_IN1,HIGH);
      digitalWrite (MOTOR_RIGHT_IN2,LOW);
      analogWrite (MOTOR_RIGHT_PWM,speedVal);
      break;

      case 'd'://右转
      digitalWrite (MOTOR_LEFT_IN1,HIGH);
      digitalWrite (MOTOR_LEFT_IN2,LOW);
      analogWrite (MOTOR_LEFT_PWM,speedVal);
      digitalWrite (MOTOR_RIGHT_IN1,LOW);
      digitalWrite (MOTOR_RIGHT_IN2,HIGH);
      analogWrite (MOTOR_RIGHT_PWM,speedVal);
      break;

      case 'x'://抱死
      motorStop();
      break;

      case ' '://空转
      digitalWrite (MOTOR_LEFT_IN1,LOW);
      digitalWrite (MOTOR_LEFT_IN2,LOW);
      digitalWrite (MOTOR_RIGHT_IN1,LOW);
      digitalWrite (MOTOR_RIGHT_IN2,LOW);
      analogWrite(MOTOR_LEFT_PWM, 0);
      analogWrite(MOTOR_RIGHT_PWM, 0);
      break;
    }
  }
}

// void JOY_ChangeSpeed (char myCmd){//直接在读数据的时候做了，用不着了
//   if (myCmd=='+'||myCmd=='-'){
//     switch (myCmd){
//       case '+':
//       ++JoyActualSpeed;
//       if(JoyActualSpeed>255) JoyActualSpeed = 255;
//       break;

//       case '-':
//       --JoyActualSpeed;
//       if(JoyActualSpeed<0) JoyActualSpeed = 0;
//       break;
//     }
//   }
// }

void handleBluetoothCommand() {
  // 一次读取一个完整指令，防止积压
  btCommand = Serial.readStringUntil('\n');
  btCommand.trim(); // 去除 '\r' 和空格

  char cmd = btCommand.charAt(0);  // 取第一个字符

  // 如果是方向指令，更新方向和心跳
  if (cmd == 'w' || cmd == 's' || cmd == 'a' || cmd == 'd' || cmd == 'x' || cmd == ' ') {
    currentDriveCmd = cmd;
    lastJoyCmdTime = millis(); // 刷新心跳时间
  }
  // 如果是速度指令，只更新速度，不改方向
  else if (cmd == '+' || cmd == '-') {
    if (cmd == '+') {
      currentDriveSpeed = constrain(currentDriveSpeed + 10, 0, 255);
    } else {
      currentDriveSpeed = constrain(currentDriveSpeed - 10, 0, 255);
    }
    // 速度改变不刷新心跳，车不会因为调速而改变运动状态
    Serial.print ("currentDriveSpeed:"); Serial.println (currentDriveSpeed);
  
  }
}