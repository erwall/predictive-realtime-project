function [ return_value ] = Kalman(option,y1,y2,u1,u2 )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here

%only designed for starting in 10 state ->16 and then staying there.
%Starting in 16 might work to but no thought have been put into it.

Phi = [3.111 0.429 1 0 0 0 0 0 0 0;
    0.08268 2.794 0 1 0 0 0 0 0 0;
    -3.182 -0.9092 0 0 1 0 0 0 0 0;
    -0.2245 -2.631 0 0 0 1 0 0 0 0;
    0.8211 0.3671 0 0 0 0 1 0 0 0;
    0.1541 0.8153 0 0 0 0 0 1 0 0;
    0.455 0.2896 0 0 0 0 0 0 1 0;
    0.02951 0.06612 0 0 0 0 0 0 0 1;
    -0.2051 -0.1762 0 0 0 0 0 0 0 0;
    -0.04261 -0.04461 0 0 0 0 0 0 0 0;
    ];

Gamma = [0.0008364 -0.0004057;
    0.0008679 -0.0001901;
    0.0009015 -0.0009729;
    0.0006347 -0.00009547;
    0.001133 0.002063;
    -0.002024 0.0000549;
    -0.001292 -0.0003008;
    0.0003297 -0.0003316;
    0 0;
    0 0;
   ];

C = [1 0 0 0 0 0 0 0 0 0;
    0 1 0 0 0 0 0 0 0 0];


Phi2 = [3.11100000000000,0.429000000000000,1,0,0,0,0,0,0,0,0,0,0.000836400000000000,-0.000405700000000000,1,0;0.0826800000000000,2.79400000000000,0,1,0,0,0,0,0,0,0,0,0.000867900000000000,-0.000190100000000000,0,1;-3.18200000000000,-0.909200000000000,0,0,1,0,0,0,0,0,0,0,0.000901500000000000,-0.000972900000000000,0,0;-0.224500000000000,-2.63100000000000,0,0,0,1,0,0,0,0,0,0,0.000634700000000000,-9.54700000000000e-05,0,0;0.821100000000000,0.367100000000000,0,0,0,0,1,0,0,0,0,0,0.00113300000000000,0.00206300000000000,0,0;0.154100000000000,0.815300000000000,0,0,0,0,0,1,0,0,0,0,-0.00202400000000000,5.49000000000000e-05,0,0;0.455000000000000,0.289600000000000,0,0,0,0,0,0,1,0,0,0,-0.00129200000000000,-0.000300800000000000,0,0;0.0295100000000000,0.0661200000000000,0,0,0,0,0,0,0,1,0,0,0.000329700000000000,-0.000331600000000000,0,0;-0.205100000000000,-0.176200000000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0;-0.0426100000000000,-0.0446100000000000,0,0,0,0,0,0,0,0,0,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0;0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0;0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0;0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0;0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0;0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1;];

Gamma2 = [0.000836400000000000,-0.000405700000000000;0.000867900000000000,-0.000190100000000000;0.000901500000000000,-0.000972900000000000;0.000634700000000000,-9.54700000000000e-05;0.00113300000000000,0.00206300000000000;-0.00202400000000000,5.49000000000000e-05;-0.00129200000000000,-0.000300800000000000;0.000329700000000000,-0.000331600000000000;0,0;0,0;0,0;0,0;0,0;0,0;0,0;0,0;];

C2 = [1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0;0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0;];

    Q = 5*diag([1,1,ones(1,8)*1]); %State covariance
    R = 1*diag([1,1]); %output covariance
    
    Q_e = 5*diag([1,1,ones(1,8)*1,0.5*ones(1,2),0.5*ones(1,2),0.1*ones(1,2)]); %State covariance
    R_e = 1*diag([1,1]); %output covariance


persistent xhat;
persistent xhatm;
persistent xhatm_e;
persistent xhat_ext;
persistent Pp;
persistent Pp_ext;
persistent K;
persistent K_e;
persistent Pm; %Should save memory allocation?
persistent Pm_e;
persistent init_10;
persistent init_16;
persistent init_extended;


if isempty(xhat) %First function call.
    xhat = zeros(10,1);
    Pp = diag(ones(1,10)*1000); %initial covariance
    xhatm = zeros(10,1);
    xhatm_e = zeros(16,1);
    K = zeros(10,2);
    K_e = zeros(16,2);
    init_10 = 0;
    init_16 = 0;
    init_extended = 0;
    Pp_ext =diag(ones(1,16)*1000); %Will be reinitalized alter
    xhat_ext = zeros(16,1);%Will be reinitalized alter
end

%Fix inputs.
y = [y1;y2];
u = [u1;u2];


if option == 1
%update prediction 10 states
    init_10 = 1;
    xhatm = zeros(10,1);
    Pm = Phi*Pp*Phi'+Q;
    K = Pm*C'*inv(C*Pm*C'+R);
    xhatm = Phi*xhat+Gamma*(u-[2.7 4.3]');
    
    %precompute
    Pp = (eye(10,10)-K*C)*Pm;
    %Symmetry
    Pp = (Pp+Pp')/2;
    
    return_value = [xhat;zeros(6,1)];
    

elseif option == 2
    %update measurement 10 states
    if ~init_10 %This might give some very minor input-output delay for first function call
                 %But doesnt let the user make mistakes.
        init_10 = 1;
       xhatm = zeros(10,1);
        Pm = Phi*Pp*Phi'+Q;
        K = Pm*C'*inv(C*Pm*C'+R);
        xhatm = Phi*xhat+Gamma*([0 0]'-[2.7 4.3]');
    
        %precompute
        Pp = (eye(10,10)-K*C)*Pm;
        %Symmetry
        Pp = (Pp+Pp')/2;
    end

    xhat = xhatm+K*(y-C*xhatm);
    return_value = [xhat;zeros(6,1)];
    
    
elseif option ==3
%update prediction 16 states
    init_16 = 1;
    if ~init_extended %initalize 
        init_extended = 1;
        xhat_ext = zeros(16,1);
        xhat_ext(1:10) = xhat;
        % Predictor should be called first.
        Pp_ext = diag([1 ,1 ,ones(1,8), 0.5*[1 1], 0.5*[1 1] , [0.1,0.1 ]]); %initial covariance
        Pp_ext(1:10,1:10) = Pp;
    end

    Pm_e = Phi2*Pp_ext*Phi2'+Q_e;
    K_e = Pm_e*C2'/(C2*Pm_e*C2'+R_e);
    xhatm_e = Phi2*xhat_ext+Gamma2*(u-[2.7 4.3]');
    
    Pp_ext = (eye(16,16)-K_e*C2)*Pm_e;
    %Symmetry
    Pp_ext = (Pp_ext+Pp_ext')/2;
    
    %xhat_ = xhat_ext;   ??
    return_value = xhat_ext;

elseif option ==4
%update meassurement 16 states
    if ~init_16  %This might give some very minor input-output delay for first function call
                 %But doesnt let the user make mistakes.
        init_16 = 1;
        if ~init_extended %initalize 
            init_extended = 1;
            xhat_ext = zeros(16,1);
            xhat_ext(1:10) = xhat;
            % Predictor should be called first.
            Pp_ext = diag([1 ,1 ,ones(1,8), 0.5*[1 1], 0.5*[1 1] , [0.1,0.1 ]]); %initial covariance
            Pp_ext(1:10,1:10) = Pp;
        end

        Pm_e = Phi2*Pp_ext*Phi2'+Q_e;
        K_e = Pm_e*C2'/(C2*Pm_e*C2'+R_e);
        xhatm_e = Phi2*xhat_ext+Gamma2*([0 0]'-[2.7 4.3]');
    
        Pp_ext = (eye(16,16)-K_e*C2)*Pm_e;
    %Symmetry
        Pp_ext = (Pp_ext+Pp_ext')/2;
    
      %  xhat_ = xhat_ext;   ??
    end
    xhat_ext = xhatm_e+K_e*(y-C2*xhatm_e);
    return_value = xhat_ext;
else %Shoudlnt happen ...
   xhat_ext = xhatm_e+K_e*(y-C2*xhatm_e);
   return_value = xhat_ext;
end

end

