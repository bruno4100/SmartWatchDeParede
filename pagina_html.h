void htmlPart1(){
  pagHtml = "<HTML> ";
pagHtml=pagHtml + "<head>";
pagHtml=pagHtml + "    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">";
pagHtml=pagHtml + "<style>";
pagHtml=pagHtml + ".vermelho{color: #ff0000;font-size:22px;font-weight:bold;}";
pagHtml=pagHtml + ".verde{color: #009900;font-size:22px;font-weight:bold;}";
pagHtml=pagHtml + ".azul{color: #000088;font-size:22px;font-weight:bold;margin-top:5px;width:400px;text-align:right;}";
pagHtml=pagHtml + ".botaoCor {  padding:20px 20px;}";

pagHtml=pagHtml + ".menuSelecionado {font-weight:bold;font-size:40px;text-decoration:none;color:black;}";
pagHtml=pagHtml + ".menuNaoSelecionado {font-weight:normal;font-size:40px;text-decoration:none;color:black;}";
pagHtml=pagHtml + ".chrt {vertical-align:bottom;}";
pagHtml=pagHtml + ".chr2 {width:10px;background-color: red;}";

pagHtml=pagHtml + "</style>";
pagHtml=pagHtml + "</head>";
pagHtml=pagHtml + "<body>";
pagHtml=pagHtml + "<BR><center>";
pagHtml=pagHtml + "<big><b>SmartWatch de Parede</b></big></center>";
pagHtml=pagHtml + "<br />";

pagHtml=pagHtml + "<script>";

pagHtml=pagHtml + "function mudaAba(aba){";
}

void htmlPart2(){
pagHtml= "switch(aba){";
pagHtml=pagHtml + "case 1:";
pagHtml=pagHtml + "document.getElementById('painelGeral').style.display='';";
pagHtml=pagHtml + "document.getElementById('painelConfig').style.display='none';";
pagHtml=pagHtml + "document.getElementById('painelAlarme').style.display='none';";
pagHtml=pagHtml + "document.getElementById('menuItemGeral').className='menuSelecionado';";
pagHtml=pagHtml + "document.getElementById('menuItemAlarme').className='menuNaoSelecionado';";
pagHtml=pagHtml + "document.getElementById('menuItemConfig').className='menuNaoSelecionado';";
pagHtml=pagHtml + "break;";

pagHtml=pagHtml + "case 2:";
pagHtml=pagHtml + "document.getElementById('painelGeral').style.display='none';";
pagHtml=pagHtml + "document.getElementById('painelConfig').style.display='';";
pagHtml=pagHtml + "document.getElementById('painelAlarme').style.display='none';";
pagHtml=pagHtml + "document.getElementById('menuItemGeral').className='menuNaoSelecionado';";
pagHtml=pagHtml + "document.getElementById('menuItemAlarme').className='menuNaoSelecionado';";
pagHtml=pagHtml + "document.getElementById('menuItemConfig').className='menuSelecionado';";
pagHtml=pagHtml + "document.getElementById('idIframe').src='http://'+window.location.hostname+'/?showScan=1';";
pagHtml=pagHtml + "break;";

pagHtml=pagHtml + "case 3:";
pagHtml=pagHtml + "document.getElementById('painelGeral').style.display='none';";
pagHtml=pagHtml + "document.getElementById('painelConfig').style.display='none';";
pagHtml=pagHtml + "document.getElementById('painelAlarme').style.display='';";
pagHtml=pagHtml + "document.getElementById('menuItemGeral').className='menuNaoSelecionado';";
pagHtml=pagHtml + "document.getElementById('menuItemAlarme').className='menuSelecionado';";
pagHtml=pagHtml + "document.getElementById('menuItemConfig').className='menuNaoSelecionado';";
pagHtml=pagHtml + "break;";

pagHtml=pagHtml + "default:";
pagHtml=pagHtml + "break;";
pagHtml=pagHtml + "}";

pagHtml=pagHtml + "} ";

//return pagHtml;
  }

void htmlPart3(){ 

  //faz a verificacao da maior e menor temp das ultimas 24h
  tempMax24h=-99;
  tempMin24h=99;

  for(int i=47; i>=0; i-- ){

if(diaTemp24h[i]>0){
  if(temp24h[i]>tempMax24h){
tempMax24h=temp24h[i];
diaMaxTemp24h=diaTemp24h[i];
mesMaxTemp24h=mesTemp24h[i];
anoMaxTemp24h=anoTemp24h[i];
horaMaxTemp24h=horaTemp24h[i];
minutoMaxTemp24h=minutoTemp24h[i];
    }

     if(temp24h[i]<tempMin24h){
tempMin24h=temp24h[i];
diaMinTemp24h=diaTemp24h[i];
mesMinTemp24h=mesTemp24h[i];
anoMinTemp24h=anoTemp24h[i];
horaMinTemp24h=horaTemp24h[i];
minutoMinTemp24h=minutoTemp24h[i];
    }
    
  }
   
  }

pagHtml = "";

pagHtml=pagHtml + "function liga(rele){";
//pagHtml=pagHtml + "window.location.href='http://'+window.location.hostname+'/?rele'+rele+'=1';";
pagHtml=pagHtml + "document.getElementById('idIframe').src='http://'+window.location.hostname+'/?rele'+rele+'=1';";
pagHtml=pagHtml + "}";
pagHtml=pagHtml + "function desliga(rele){";
//pagHtml=pagHtml + "window.location.href='http://'+window.location.hostname+'/?rele'+rele+'=0';";
pagHtml=pagHtml + "document.getElementById('idIframe').src='http://'+window.location.hostname+'/?rele'+rele+'=0';";
pagHtml=pagHtml + "}";
pagHtml=pagHtml + "function cor(valor){";
pagHtml=pagHtml + "retorno.location='http://'+document.getElementsByTagName(\"input\")[0].value+'/'+valor;";
pagHtml=pagHtml + "}";
pagHtml=pagHtml + "function vel(){";
pagHtml=pagHtml + "retorno.location='http://'+document.getElementsByTagName(\"input\")[0].value+'/speed'+document.getElementsByTagName(\"input\")[1].value+'z4ledPower'+document.getElementsByTagName(\"input\")[2].value+'z3';";
pagHtml=pagHtml + "}";
pagHtml=pagHtml + "";
pagHtml=pagHtml + "function corPersonal(){";
pagHtml=pagHtml + "retorno.location='http://'+document.getElementsByTagName(\"input\")[0].value+'/red'+document.getElementsByTagName(\"input\")[3].value+'zRedgreen'+document.getElementsByTagName(\"input\")[4].value+'zGreenblue'+document.getElementsByTagName(\"input\")[5].value+'zBlue';";
pagHtml=pagHtml + "}";
pagHtml=pagHtml + "";
pagHtml=pagHtml + "</script>";
pagHtml=pagHtml + "";
pagHtml=pagHtml + "";
pagHtml=pagHtml + "<center>";
pagHtml=pagHtml + "<div>";
pagHtml=pagHtml + "<a href=\"#\" id=\"menuItemGeral\" class=\"menuSelecionado\" onclick=\"mudaAba(1);\">Geral</a> | <a href=\"#\" id=\"menuItemAlarme\" class=\"menuNaoSelecionado\" onclick=\"mudaAba(3);\">Automações</a> | <a href=\"#\" id=\"menuItemConfig\" class=\"menuNaoSelecionado\" onclick=\"mudaAba(2);\">Configurações</a>";
pagHtml=pagHtml + "</div>    ";
pagHtml=pagHtml + "</center>";
pagHtml=pagHtml + "<hr> ";

pagHtml= pagHtml + "<div id=\"painelGeral\" style=\"display:block;\">";
pagHtml=pagHtml + "<center>";
pagHtml=pagHtml + "Relógio funcionando desde <b>"+String(dia1aSincronia)+"/"+String(mes1aSincronia)+"/"+String(ano1aSincronia)+" "+String(hora1aSincronia)+":"+(minuto1aSincronia<10?"0":"")+String(minuto1aSincronia)+"</b><br />";
pagHtml=pagHtml + "Última sincronia NTP: <b>"+String(diaUltimaSincronia)+"/"+String(mesUltimaSincronia)+"/"+String(anoUltimaSincronia)+" "+String(horaUltimaSincronia)+":"+(minutoUltimaSincronia<10?"0":"")+String(minutoUltimaSincronia)+"</b><br />";
pagHtml=pagHtml + "<br />Horário e data atuais: <b>"+(dig1)+(dig2)+":"+(dig3)+(dig4)+"</b> "+dia+"/"+mes+"/"+ano+" <br />Temperatura atual: <b>"+temperature+" &deg;C</b><br /><br />";
pagHtml=pagHtml + "<b>Máx temperatura</b><br />";
pagHtml=pagHtml + "<b>Últimas 24h: </b>"+tempMax24h+" &deg;C, em "+diaMaxTemp24h+"/"+mesMaxTemp24h+"/"+anoMaxTemp24h+" "+horaMaxTemp24h+":"+(minutoMaxTemp24h<10?"0":"")+String(minutoMaxTemp24h)+"<br />";
pagHtml=pagHtml + "<b>Desde o início: </b>"+temperatureMaxEver+" &deg;C, em "+diaMaxTempEver+"/"+mesMaxTempEver+"/"+anoMaxTempEver+" "+horaMaxTempEver+":"+(minutoMaxTempEver<10?"0":"")+String(minutoMaxTempEver)+"<br />";
pagHtml=pagHtml + "<br />";
pagHtml=pagHtml + "<b>Mín temperatura</b><br />";
pagHtml=pagHtml + "<b>Últimas 24h: </b>"+tempMin24h+" &deg;C, em "+diaMinTemp24h+"/"+mesMinTemp24h+"/"+anoMinTemp24h+" "+horaMinTemp24h+":"+(minutoMinTemp24h<10?"0":"")+String(minutoMinTemp24h)+"<br />";
pagHtml=pagHtml + "<b>Desde o início: </b>"+temperatureMinEver+" &deg;C, em "+diaMinTempEver+"/"+mesMinTempEver+"/"+anoMinTempEver+" "+horaMinTempEver+":"+(minutoMinTempEver<10?"0":"")+String(minutoMinTempEver)+"<br />";
pagHtml=pagHtml + "<br/><b>Dados da temperatura das ultimas 24h:</b><br />";
}

void htmlPart4(){

pagHtml= "<table background=\"https://cdn.printablepaper.net/samples/grid-portrait-letter-2-noindex.png\" style=\"height:285px;margin-top:5px;\"><tr>";
pagHtml=pagHtml + "<td class=\"chrt\"><div style=\"height:100%;\">50</div></td>";
//pagHtml=pagHtml + "<table><tr>";
//pagHtml=pagHtml + "<td ><div >50</div></td>";

for(int i=47;i>=0;i--){
  //pagHtml=pagHtml + temp24h[i]+" &deg;C em "+diaTemp24h[i]+"/"+mesTemp24h[i]+"/"+anoTemp24h[i]+" "+horaTemp24h[i]+":"+minutoTemp24h[i]+" <br />";

      //pagHtml=pagHtml + "<td rowspan=\"5\" class=\"tdchart\"><div style=\"width:10px;height:"+(temp24h[i] * 5)+"px;background-color: red;\"></div></td>";
        pagHtml=pagHtml + "<td rowspan=5 class=\"chrt\"><div class=\"chr2\" style=\"height:"+(temp24h[i] * 5)+"px;\"></div></td>";
  
  }
pagHtml=pagHtml + "</tr><tr><td class=\"chrt\"><div style=\"height:85%;\">38</div></td></tr>";
pagHtml=pagHtml + "<tr><td class=\"chrt\"><div style=\"height:70%;\">25</div></td></tr>";
pagHtml=pagHtml + "<tr><td class=\"chrt\"><div style=\"height:50%;\">13</div></td></tr>";
pagHtml=pagHtml + "<tr><td class=\"chrt\">0</td></tr>";
pagHtml=pagHtml + "<tr><td colspan=\"23\" style=\"height:25px;\">-24h</td><td colspan=\"24\" style=\"height:25px;\">-12h</td><td colspan=\"2\" style=\"height:25px;\">-1h</td></tr></table>";

//return pagHtml;

 }

void htmlPart5(){ 
  
pagHtml="";
pagHtml=pagHtml + "<br/><br/>";
pagHtml=pagHtml + "<div class=\"azul\">Rele 1 <small><small>("+(estadoRele1?"Ligado":"Desligado")+"):</small></small> ";
pagHtml=pagHtml + "<button class=\"verde\" onclick=\"liga(1)\" >Liga</button>";
pagHtml=pagHtml + "<button class=\"vermelho\" onclick=\"desliga(1)\" >Desliga</button>";
pagHtml=pagHtml + "</div>";
pagHtml=pagHtml + "<div class=\"azul\">Rele 2 <small><small>("+(estadoRele2?"Ligado":"Desligado")+"):</small></small> ";
pagHtml=pagHtml + "<button class=\"verde\" onclick=\"liga(2)\" >Liga</button>";
pagHtml=pagHtml + "<button class=\"vermelho\" onclick=\"desliga(2)\" >Desliga</button>";
pagHtml=pagHtml + "</div>";
pagHtml=pagHtml + "</center>";
pagHtml=pagHtml + "</div>";
pagHtml=pagHtml + "";
pagHtml=pagHtml + "<div id=\"painelConfig\" style=\"display:none;\">";
pagHtml=pagHtml + "<center>";
pagHtml=pagHtml + "<br />";
pagHtml=pagHtml + "";
pagHtml=pagHtml + "<table><tr><td><form method=\"GET\">";
pagHtml=pagHtml + "    <b>Rede para acesso à internet:</b><br />";
pagHtml=pagHtml + "  SSID: <input type=\"text\" name=\"wlanSsid\" value=\""+esid+"\"><br>";
pagHtml=pagHtml + "  Password: <input type=\"text\" name=\"wlanPass\" value=\""+epass+"\"><br>";
pagHtml=pagHtml + "  <input type=\"submit\" value=\"Aplicar\">";
pagHtml=pagHtml + "</form></td><td>Redes disponíveis (potencia do sinal: -80[fraco] -50[forte]):<br /><iframe src=\"\" id=\"idIframe\" name=\"nameIframe\"></iframe></td></tr></table>";
pagHtml=pagHtml + "<br />";
pagHtml=pagHtml + "IP obtido na rede para acesso à internet: <b>"+WiFi.localIP().toString()+"</b> <br />";
pagHtml=pagHtml + "";
pagHtml=pagHtml + "<br />";
pagHtml=pagHtml + "<form method=\"GET\">";
pagHtml=pagHtml + "<b>Data de horário de verão (dd/mm):</b><br />";
pagHtml=pagHtml + "Início: <input type=\"text\" name=\"diaInicioHverao\" size=\"2\" maxlength=\"2\" value=\""+diaInicioHverao+"\">/<input type=\"text\" name=\"mesInicioHverao\" size=\"2\" maxlength=\"2\" value=\""+mesInicioHverao+"\"><br />";
pagHtml=pagHtml + "Término: <input type=\"text\" name=\"diaTerminoHverao\" size=\"2\" maxlength=\"2\" value=\""+diaTerminoHverao+"\">/<input type=\"text\" name=\"mesTerminoHverao\" size=\"2\" maxlength=\"2\" value=\""+mesTerminoHverao+"\"><br />";
pagHtml=pagHtml + " <input type=\"submit\" value=\"Aplicar\">";
pagHtml=pagHtml + "</form>";
pagHtml=pagHtml + "<br><big>";
}

void htmlPart6(){ 
pagHtml= "<form method=\"GET\"><table border=0><tr><td colspan=\"2\">";
pagHtml=pagHtml + "<b>Horário de verão ativado?</b></td></tr>";
pagHtml=pagHtml + "<tr><td>";
pagHtml=pagHtml + "NÂO<input type=\"range\" name=\"hVerao\" id=\"hVeraoId\" min=\"0\" max=\"1\" value=\""+horarioVeraoAtivo+"\">";
pagHtml=pagHtml + "SIM<br>";
pagHtml=pagHtml + "</td><td rowspan=2><button class=\"botaoCor\" style=\"";
pagHtml=pagHtml + "padding:4px 2px;color:#000000;\" onclick=\"vel()\" > Aplicar </button></td></tr></table></form>";
pagHtml=pagHtml + "";
pagHtml=pagHtml + "</big></center>";
pagHtml=pagHtml + "</div>";

pagHtml=pagHtml + "<div id=\"painelAlarme\" style=\"display:none;\">";
pagHtml=pagHtml + "<center><form method=\"GET\"><big>";
pagHtml=pagHtml + "<b>Automações:</b></big><br><br>";

pagHtml=pagHtml + "Relê 1 <br>";
pagHtml=pagHtml + "<table border=0 cellpadding=\"4\"><tr>";
pagHtml=pagHtml + "<td><input type=\"radio\" name=\"autoTemp1\" value=\"0\" "+(autoTemp1==0?"checked=\"true\"":"")+"> Não automatizar por temperatura <br>";
pagHtml=pagHtml + "<input type=\"radio\" name=\"autoTemp1\" value=\"1\" "+(autoTemp1==1?"checked=\"true\"":"")+"> <b>Acionar</b> quando maior ou igual a: <br>";
pagHtml=pagHtml + "<input type=\"radio\" name=\"autoTemp1\" value=\"2\" "+(autoTemp1==2?"checked=\"true\"":"")+"> <b>Desligar</b> quando maior ou igual a:</td>";
pagHtml=pagHtml + "<td bgcolor=\"#bbbbbb\"><b>Temperatura: </b><br><input style=\"width:50px;\" type=\"number\" name=\"temp1\" min=\"-99\" max=\"99\" value=\""+temp1+"\"> &deg;C";
pagHtml=pagHtml + "</td><td></td>";
pagHtml=pagHtml + "</tr>";
pagHtml=pagHtml + "<tr>";
pagHtml=pagHtml + "<td><input type=\"radio\" name=\"autoClock1\" value=\"0\" "+(autoClock1 ==0?"checked=\"true\"":"")+"> Não automatizar por tempo <br>";
pagHtml=pagHtml + "<input type=\"radio\" name=\"autoClock1\" value=\"1\" "+(autoClock1 ==1?"checked=\"true\"":"")+"> <b>Acionar</b> no intervalo: <br>";
pagHtml=pagHtml + "<input type=\"radio\" name=\"autoClock1\" value=\"2\" "+(autoClock1 ==2?"checked=\"true\"":"")+"> <b>Desligar</b> no intervalo: </td>";
pagHtml=pagHtml + "<td bgcolor=\"#bbbbbb\"><b>Início: </b><br><input style=\"width:50px;\" type=\"number\" name=\"hrInicio1\" min=\"0\" max=\"23\"  value=\""+hrInicio1+"\" > : <input style=\"width:50px;\" type=\"number\" name=\"minInicio1\" value=\""+minInicio1+"\" min=\"0\" max=\"59\"> </td>";
pagHtml=pagHtml + "<td bgcolor=\"#bbbbbb\"><b>Término: </b><br><input style=\"width:50px;\" type=\"number\" name=\"hrTermino1\" min=\"0\" max=\"23\" value=\""+hrTermino1+"\"> : <input style=\"width:50px;\" type=\"number\" name=\"minTermino1\" min=\"0\" max=\"59\" value=\""+minTermino1+"\"> </td>";
}

void htmlPart7(){ 
pagHtml= "</tr>";
pagHtml=pagHtml + "</table>";
pagHtml=pagHtml + "<br>";
pagHtml=pagHtml + "Relê 2 <br>";
pagHtml=pagHtml + "<table border=0 cellpadding=\"4\"><tr>";
pagHtml=pagHtml + "<td><input type=\"radio\" name=\"autoTemp2\" value=\"0\" "+(autoTemp2==0?"checked=\"true\"":"")+"> Não automatizar por temperatura <br>";
pagHtml=pagHtml + "<input type=\"radio\" name=\"autoTemp2\" value=\"1\" "+(autoTemp2==1?"checked=\"true\"":"")+"> <b>Acionar</b> quando maior ou igual a: <br>";
pagHtml=pagHtml + "<input type=\"radio\" name=\"autoTemp2\" value=\"2\" "+(autoTemp2==2?"checked=\"true\"":"")+"> <b>Desligar</b> quando maior ou igual a:</td>";
pagHtml=pagHtml + "<td bgcolor=\"#bbbbbb\"><b>Temperatura: </b><br><input style=\"width:50px;\" type=\"number\" name=\"temp2\" min=\"-99\" max=\"99\" value=\""+temp2+"\"> &deg;C";
pagHtml=pagHtml + "</td><td></td>";
pagHtml=pagHtml + "</tr>";
pagHtml=pagHtml + "<tr>";
pagHtml=pagHtml + "<td><input type=\"radio\" name=\"autoClock2\" value=\"0\" "+(autoClock2 ==0?"checked=\"true\"":"")+"> Não automatizar por tempo <br>";
pagHtml=pagHtml + "<input type=\"radio\" name=\"autoClock2\" value=\"1\" "+(autoClock2 ==1?"checked=\"true\"":"")+"> <b>Acionar</b> no intervalo: <br>";
pagHtml=pagHtml + "<input type=\"radio\" name=\"autoClock2\" value=\"2\" "+(autoClock2 ==2?"checked=\"true\"":"")+"> <b>Desligar</b> no intervalo: </td>";
pagHtml=pagHtml + "<td bgcolor=\"#bbbbbb\"><b>Início: </b><br><input style=\"width:50px;\" type=\"number\" name=\"hrInicio2\" min=\"0\" max=\"23\" value=\""+hrInicio2+"\"> : <input style=\"width:50px;\" type=\"number\" name=\"minInicio2\" min=\"0\" max=\"59\" value=\""+minInicio2+"\"> </td>";
pagHtml=pagHtml + "<td bgcolor=\"#bbbbbb\"><b>Término: </b><br><input style=\"width:50px;\" type=\"number\" name=\"hrTermino2\" min=\"0\" max=\"23\" value=\""+hrTermino2+"\"> : <input style=\"width:50px;\" type=\"number\" name=\"minTermino2\" min=\"0\" max=\"59\" value=\""+minTermino2+"\"> </td>";
pagHtml=pagHtml + "</tr>";
pagHtml=pagHtml + "</table>";
pagHtml=pagHtml + "<br>";
pagHtml=pagHtml + "<input type=\"submit\" value=\"Aplicar\">";
pagHtml=pagHtml + "</form>";
pagHtml=pagHtml + "</center>";

pagHtml=pagHtml + "</div>";
pagHtml=pagHtml + "<br><center><div style=\"color:gray;\">&reg; Feito por Bruno Luchini Lucato - Trabalho de Graduação - Gestão de TI - Fatec Campinas 2018</div></center></body> </HTML>";

//return pagHtml;
}
