#include "gui.hpp"

extern Variables *var;

void display_task(void *pvParameters){

	Serial.print("display_task running on core ");
	Serial.println(xPortGetCoreID());
	GUI gui;
	Adafruit_SSD1306 display(128, 32, &Wire);

	// ----- Display Setup -----
	if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
	{ 
		// Address 0x3C for 128x32
		Serial.println(F("SSD1306 allocation failed"));
		exit(1);
	}

	display.display();
	delay(500);
	display.setTextSize(0);
	display.setTextColor(WHITE);
	display.setRotation(0);


	//loop
	for(;;){
		gui.button_logic(var);
		gui.page_ui(var, &display);	
		vTaskDelay(pdMS_TO_TICKS(500)); //ticks para ms
	}

}


// ----- change arrow position -----
void GUI::arrow_mov(Variables *var) {
	if (var->arrow == ARROW_DOWN) {
		var->arrow = ARROW_UP;
	} else {
		var->arrow = ARROW_DOWN;
	}
}
// ----- Button logic -----
void GUI::button_logic(Variables *var) {
	var->currentState = digitalRead(BUTTON_PIN);
	if (var->lastState == HIGH && var->currentState == LOW)
	{
		Serial.println("Botao Apertado");
		var->timerMillisButton = millis();


		// ----- GO BACKWORDS -----
		if(millis() - var->timerBackButton < 150) //button pressed twice
		{
			if (var->page == DELAY_PAGE)
				var->page = PARAM_PAGE;

			else if (var->page == TEST_PAGE)
				var->page = FIRST_PAGE;

			else if (var->page == ENDED_TEST_PAGE)
				var->page = FIRST_PAGE;

			var->page = var->page - 1;
			
			if (var->page < FIRST_PAGE)
				var->page = FIRST_PAGE;
		}

		while (digitalRead(BUTTON_PIN) == LOW)
			;

		// ----- GO FOWARD -----
		if (millis() - var->timerMillisButton >= 800)
		{
			if (var->page == FIRST_PAGE) {
				if (var->arrow == ARROW_UP) {
					var->page = TEST_PAGE;
				} else {
					var->page = PARAM_PAGE;
				}
			}
			else if (var->page == PARAM_PAGE) {
				if(var->arrow == ARROW_UP) {
					var->page = OFFSET_PAGE;
				} else if(var->arrow == ARROW_DOWN) {
					var->page = DELAY_PAGE;
				}
			}
			else if (var->page == OFFSET_PAGE) {
				if(var->arrow == ARROW_UP) {
					var->offset_stop += 1;
				} else if(var->arrow == ARROW_DOWN) {
					var->offset_stop -= 1;
					if(var->offset_stop < 0){
						var->offset_stop = 0;
					}
				}
				EEPROM.write(OFFSET_ADDRESS, var->offset_stop);
				EEPROM.commit();
			}
			else if (var->page == DELAY_PAGE) {
				if(var->arrow == ARROW_UP) {
					var->delay_acc += 10;
				}else if (var->arrow == ARROW_DOWN) {
					var->delay_acc -= 10;
					if(var->delay_acc < 0){
						var->delay_acc = 0;
					}
				}
				EEPROM.write(DELAY_ADDRESS, var->delay_acc);
				EEPROM.commit();
			} else {
				var->page = FIRST_PAGE;
			}
		}

		// ----- CHANGE ARROW POSITION -----
		else
		{
			if (var->page == ENDED_TEST_PAGE)
				var->page = FIRST_PAGE;

			arrow_mov(var);
		}

		var->timerBackButton = millis();

	}

	var->lastState = var->currentState;
}

void GUI::regressive_counter_seconds(int numberOfCounter, Adafruit_SSD1306 *display) {
	display->setCursor(30, 10);
	Serial.println("regressive counter function");

	for(int i = numberOfCounter; i > 0; i--) {
		display->clearDisplay();
		display->print(i);
		Serial.println(i);
		display->display();
		delay(1000);
	}

	display->setCursor(10, 10);
	display->clearDisplay();
	display->print("Test Started!");
	display->display();
	Serial.println("Test Started!");
	delay(1000);
}

// ----- PAGE UI -----
void GUI::page_ui(Variables *var, Adafruit_SSD1306 *display) {
	if (var->page == FIRST_PAGE) {
		display->clearDisplay();
		display->setCursor(50, 0);
		display->print("EQUINOS"); 
		display->setCursor(10, 10);
		display->print("Modo de teste");
		display->setCursor(10, 20);
		display->print("mudar parametros"); 
		display->setCursor(0, var->arrow);
		display->print(">");
		display->display();
	} else if (var->page == PARAM_PAGE) {
		display->clearDisplay();
		display->setCursor(30, 0);
		display->print("PARAMETROS"); 
		display->setCursor(10, 10);
		display->print("Sensibilidade"); 
		display->setCursor(10, 20);
		display->print("Amostragem");
		display->setCursor(0, var->arrow);
		display->print(">");
		display->display();
	} else if (var->page == OFFSET_PAGE) {
		display->clearDisplay();
		display->setCursor(10, 0);
		display->print("Sensibilidade = "); 
		display->setCursor(110, 0);
		display->print(var->offset_stop); 
		display->setCursor(10, 10);
		display->print("+"); 
		display->setCursor(10, 20);
		display->print("-"); 
		display->setCursor(0, var->arrow);
		display->print(">");
		display->display();
	} else if (var->page == DELAY_PAGE) {
		display->clearDisplay();
		display->setCursor(10, 0);
		display->print("Amostragem = ");
		display->setCursor(90, 0);
		display->print(var->delay_acc); 
		display->setCursor(10, 10);
		display->print("+"); 
		display->setCursor(10, 20);
		display->print("-"); 
		display->setCursor(0, var->arrow);
		display->print(">");
		display->display();
	} else if (var->page == TEST_PAGE) {
		display->clearDisplay();
		display->setCursor(20, 0);
		display->print("TESTE INICIADO");
		display->setCursor(10, 20);
		display->print("media: ");
		display->setCursor(50, 20);
		display->print(var->average); 
		display->display();
	} else if (var->page == ENDED_TEST_PAGE) {
		display->clearDisplay();
		display->setCursor(20, 0);
		display->print("TESTE FINALIZADO");
		display->setCursor(10, 20);
		display->print("media: ");
		display->setCursor(50, 20);
		display->print(var->average); 
		display->setCursor(10, 40);
		display->print("max: ");
		display->setCursor(50, 40);
		display->print(var->body.max_z); 
		display->display();
	}
}
