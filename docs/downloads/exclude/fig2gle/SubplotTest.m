%% SubplotTest
% This is a sample figure for testing the MATLAB/Octave figure to GLE routine.
x = 0:0.01:10;
xb = 0:0.05:10;
xc = 0:10;
xd = logspace(0,4,21);
y1 = sin(x);
y2 = cos(xb);
y3 = cos(x);
y4 = -2+sin(xc);
% subplot 1
subplot(2,2,1); 
plot(x,y1,'-.',xb,y2,'--m'); 
grid
title('Plot 1')
xlabel('$x_1$'); 
ylabel('$y_1$ and $y_2$'); 
legend('$y_1$','$y_2$');
% subplot 2
subplot(2,2,2); 
semilogx(xd,xd,'-^'); 
grid
title('Plot 2')
xlabel('$x_2$'); 
ylabel('$y_2$');
% subplot 3
subplot(2,2,3); 
plot(x,y3,xc,y4,'x',xb,2*y2,'LineWidth',2); 
title('Plot 3')
xlabel('$x_3$'); 
ylabel('$y_3$, $y_4$, and $y_2$');
legend('$y_3$','$y_4$','$y_2$')
% subplot 4
subplot(2,2,4); 
plot(xc,y4,'+'); 
title('Plot 4')
xlabel('$x_4$'); 
ylabel('$y_4$');
