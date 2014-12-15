clear all; 
close all; clc

fid = fopen('Datalog.txt');

tline = fgetl(fid);
i = 1;
lines = 0;

while ischar(tline)
        if strcmp(tline,'New Data');
            lines = lines + 1;
        end
    array{i} = tline;       %collect lines in array
    tline = fgetl(fid);
    i = i + 1;
    
    
end

fclose(fid);

data(682,lines) = {0};
a = 5;


for k = 1:lines
 
    x(k) = array(a-3);
    y(k) = array(a-2);
    aang(k) = array(a-1);
    for i = 5:4+681
        
        data(i-3,k) = array(a);
        a = a + 1;
    end
   a = a + 5;

end

clear array

data = str2double(data); %convert to double



x = str2double(x); 
y = str2double(y);
aang = str2double(aang);


theta = linspace(-2.0944,2.0944,682);

i = 1;
col = 'bgrcmykbgrcmykbgrcmykbgrcmyk';
a=1;
drift = 0;


plot(x,y)
for j=1:lines

    while(i < length(data)-1)
        rad = data(i,j);    
        i = i+1;
        
        cartesianx = (cos(theta(i)+aang(j)+pi/2)*rad) + x(j);
        cartesiany = (sin(theta(i)+aang(j)+pi/2)*rad) + y(j);

        hold on
        plot(cartesianx,cartesiany,'.')

    end
    
    drawnow

    disp(j)
    i = 1;
end

xlabel('x coordinates [mm]')
ylabel('y coordinates [mm]')

saveas(gcf,'Dataplot.jpg')
