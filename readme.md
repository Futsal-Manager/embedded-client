https://github.com/Futsal-Manager/embedded-client.git

1일차 
 -Init
1) clock 설정
2) GPIO 설정
3) 인터럽트 설정
4) UART2 설정 (블루투스)
5) TIM설정 (딜레이 구현)
6) UART4 설정 (서보 모터)

2일차 
 -서보 모터 설정함수 작성
1) Angle Limit 설정함수 : x축: 60도~180도, Y축: 105도~ 195도(모터 기준 각도)
2) Status return level 설정, false값 설정, Half duplex type을 사용하지 않음.(속도 저하, 불필요)
3) Motor ID 설정 Y축 :01 X축:02
4) Angle값, RPM값 을 인자로 받아 자동 패킷 생성 함수 작성(각도, RPM 입력만으로 모터 제어가능)

