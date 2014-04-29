function matlab_communication(s)
    pause on;
    set(s,'BaudRate',115200);
    fopen(s);
    %set(s, 'Timeout',10000);
    
    n = 800;
    % prefill vectors
    h1 = zeros(1,n);
    h2 = zeros(1,n);
    u = zeros(1,n);
    e = zeros(1,n);
    t = zeros(1,n);
    
    for i=1:n
        pause(0.01);
        tmp = fscanf(s, '%d');
        disp(['tank #1: ' num2str(tmp)]);
        h1(i) = tmp;
        
        tmp = fscanf(s, '%d');
        disp(['tank #2: ' num2str(tmp)]);
        h2(i) = tmp;
        
        tmp = fscanf(s, '%d');
        disp(['error: ' num2str(tmp)]);
        e(i) = tmp;
        
        tmp = fscanf(s, '%d');
        disp(['output: ' num2str(tmp)]);
        u(i) = tmp;
        
        t(i) = i;

        plot(t,h1,t,h2,t,e,t,u/8);
        xlabel('Antal Samplingar');
        ylabel('vattennivå');
        legend('tank1','tank2','felvärde','styrsignalen','Location','North');
        grid on;
    end
    fclose(s);
    delete(s);
    clear s;
end