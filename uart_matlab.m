clc; 
clear; 
close all;

s = serialport("COM11",57600); %configuracion del serial
muestras = 20000; %cantidad de muestras
n = 1;

datos = cell(1, 6); %celda con 6 elementos
for k=1:6
    datos{k} = zeros(1, muestras);
end


%% figura
figure("Name", "Voltaje");
title("VOLTAJE DEL ADC", "Interpreter", "latex", "FontSize", 12);
xlabel("T",'Interpreter', 'latex', 'FontSize',10);
ylabel("V", "Interpreter", "latex", "FontSize", 10);
grid on; 
hold on;
tiempo = 1:1:muestras;
ylim([0 3.5]);
xlim([0 muestras]);

adc0 = animatedline('Color', 'm'); %crear una linea animada
adc1 = animatedline('Color', 'r');
adc2 = animatedline('Color', 'y');
adc3 = animatedline('Color', 'g');
adc4 = animatedline('Color', 'b');
adc5 = animatedline('Color', 'k');

legend("1", "2", "3", "4", "5", "6");

%% adc
while true
    %readline para leer texto (char)
    %con str2double se convierte la cadena de caracteres (que se recibe) a
    %un dato de tipo double 
    datos{1}(n) = str2double(readline(s)); 
    datos{2}(n) = str2double(readline(s));
    datos{3}(n) = str2double(readline(s));
    datos{4}(n) = str2double(readline(s));
    datos{5}(n) = str2double(readline(s));
    datos{6}(n) = str2double(readline(s));
    
    voltaje0 = (datos{1}(n) * 3.3)/ 4095; %para convertir a voltaje
    voltaje1 = (datos{2}(n) * 3.3)/ 4095;
    voltaje2 = (datos{3}(n) * 3.3)/ 4095;
    voltaje3 = (datos{4}(n) * 3.3)/ 4095;
    voltaje4 = (datos{5}(n) * 3.3)/ 4095;
    voltaje5 = (datos{6}(n) * 3.3)/ 4095;
    
    %con la funcion addpoints se va graficando en tiempo real
    addpoints(adc0, time(n), voltaje0);
    addpoints(adc1, time(n), voltaje1);
    addpoints(adc2, time(n), voltaje2); 
    addpoints(adc3, time(n), voltaje3);
    addpoints(adc4, time(n), voltaje4);
    addpoints(adc5, time(n), voltaje5);
    drawnow %para actualizar la figura en tiempo real

    n = n + 1;
    
    if n == muestras %para cuando se alcanza el max de muestras
        break
    end
end

