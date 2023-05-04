
U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI *oledSetup(uint8_t scl,uint8_t sda,uint8_t cs,uint8_t dc,uint8_t reset){
  U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI *oled = new U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI(U8G2_R0,scl,sda,cs,dc,reset);
  oled->begin();
  oled->setFont(u8g2_font_ncenB08_tr);
  return oled;
}

void printText(U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI *oled,char *text){
  //输入的是要显示的全部文字，带换行的，然后遍历文本，把行给换了。
  char rowText[64] = "";
  uint8_t row=0,pR=0,pT = 0;
  while(text[pT]!='\0'&&p<1024){
    if(text[pT]!='\n')rowText[pR++]=text[pT++];
    else{
      pT++;
      row++;
      rowText[pR]='\0';
      oled->drawStr(0,row*10,rowText);
      pR=0;
    }
  }
}
//  oled->setFont(u8g2_font_ncenB08_tr);
//  oled->drawStr(0,min(10*line,60),text);
//  //oled->sendBuffer();


void printData(U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI *oled,char *text,void **datas,byte *type){
  char rowText1[16] = "",rowText2[16] = "",rowText[64];
  uint8_t row=0,pR=0,pT=0,n=0;
  while(text[pT]!='\0'&&p<1024){
    if(text[pT]!='\n'){
      if(n==0){
        rowText1[pR++]=text[pT++];
      }else{
        rowText2[pR++]=text[pT++];
      }
    }else{
      if(n==0){
        n++;
        rowText1[pR]='\0';
      }else{
        n=0;
        rowText2[pR]='\0';
        
        if(type[row] == 0) sprintf(rowText,"%s%d%s",rowText1,*(int *)datas[row],rowText2);
        else if(type[row] == 1) sprintf(rowText,"%s%d%s",rowText1,*(long *)datas[row],rowText2);
        else if(type[row] == 2) sprintf(rowText,"%s%f%s",rowText1,*(float *)datas[row],rowText2);
        //else sprintf(rowText,"111");
        else sprintf(rowText,"%s%f%s",rowText1,*(double *)datas[row],rowText2);
        
        row++;
        oled->drawStr(0,row*10,rowText);
      }
      pR=0;
      pT++;
    }
//    if(text[pT]!='\n'&&n==0)rowText[pR++]=text[pT++];
//    else{
//      pT++;
//      row++;
//      rowText[pR+1]='\0';
//      oled->drawStr(0,row*10,rowText);
//      pR=0;
//    }
//  }
  }
}

//void memory(U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI *oled,)


void draw(U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI *oled){
  oled->sendBuffer();
  oled->clearBuffer();
}
