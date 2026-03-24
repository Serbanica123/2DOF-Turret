A = [ 9.99161908e-01  1.70330123e-02 -1.88381569e-02  5.09442854e-03  2.52792453e-02  4.44375598e-03;
      5.47920293e-03  8.36981133e-01  4.83058621e-02  9.69110236e-02 -1.83829679e-01  4.28915309e-02;
     -3.49005349e-03  1.05644252e-01  9.40218584e-01 -6.95872669e-02  1.14226135e-02  1.74411160e-01;
      4.00817441e-03 -1.37001376e-01  2.84920821e-01  8.91117137e-01  8.51020660e-02 -1.50876747e-01;
      8.45094868e-04  9.33982860e-02 -4.20533349e-02 -1.18452180e-01  8.52561988e-01  3.40137575e-02;
     -3.76943612e-03  9.08614564e-02 -2.49374574e-01  1.65173685e-01  1.89093947e-01  8.47753831e-01 ];

B = [ -2.12541888e-03 -1.58764869e-04;
       5.04129257e-03  6.59865734e-03;
      -1.40449877e-03 -9.21113065e-05;
       6.35606745e-03  1.10096454e-03;
       2.49018841e-04 -2.92482646e-03;
      -6.72410067e-03 -8.74243802e-04 ];

C = [ -118.27407684    8.59317752   -8.76264576    6.5566347    30.53790012   19.60851764;
        1.27570958  -39.02837142  -27.12219675   31.82286319   -2.40623232    8.11306972 ];

D = zeros(2,2);

dt = 0.02;           % sample time
n = size(A,1);       
m = size(B,2);
p = size(C,1);

%% LQR design (smaller Q for stability)
Q = 10*(C'*C);       
R = 45*eye(2);
[K,~,~] = dlqr(A,B,Q,R);

%% Observer design
cl_poles = eig(A-B*K);
observer_poles = cl_poles*0.7;   % slightly slower than controller
L = place(A', C', observer_poles)';

control_data = struct();

% --- Model ---
control_data.model.A  = A;
control_data.model.B  = B;
control_data.model.C  = C;
control_data.model.D  = D;
control_data.model.dt = dt;

% --- Dimensions (explicit, helps C++) ---
control_data.dims.n = n;   % states
control_data.dims.m = m;   % inputs
control_data.dims.p = p;   % outputs

% --- LQR ---
control_data.lqr.K = K;
control_data.lqr.Q = Q;
control_data.lqr.R = R;

% --- Observer ---
control_data.observer.L = L;
control_data.observer.poles = observer_poles;

% --- Metadata ---
control_data.meta.type = "Discrete LQR + Luenberger";
control_data.meta.generated_by = "MATLAB";
control_data.meta.date = datestr(now, 'yyyy-mm-dd HH:MM:SS');

% Encode to JSON (pretty printed)
json_str = jsonencode(control_data, 'PrettyPrint', true);

% Write to file
filename = "lqr_observer_config.json";
fid = fopen(filename, 'w');
fwrite(fid, json_str, 'char');
fclose(fid);

disp("Controller exported to " + filename);

% %% Steady-state for constant reference
% r_const = [90; -90];             % constant reference
% M = [eye(n)-A, -B;
%      C,        zeros(p,m)];
% sol  = M \ [zeros(n,1); r_const];
% x_ss = sol(1:n);
% u_ss = sol(n+1:end);
% 
% %% Simulation setup
% T  = 0:dt:10;
% N  = length(T);
% 
% x    = zeros(n,1);   % true state
% xhat = x_ss;         % start observer at steady-state
% y_hist    = zeros(p,N);
% x_hist    = zeros(n,N);
% xhat_hist = zeros(n,N);
% u_hist    = zeros(m,N);
% 
% %% Simulation loop
% for k = 1:N
%     % Control law using observer
%     u = u_ss - K*(xhat - x_ss);
% 
%     % Plant update
%     x = A*x + B*u;
%     y = C*x;
% 
%     % Observer update in error coordinates
%     xhat = A*xhat + B*u + L*(y - C*xhat);
% 
%     % Store
%     x_hist(:,k)    = x;
%     xhat_hist(:,k) = xhat;
%     y_hist(:,k)    = y;
%     u_hist(:,k)    = u;
% end
% 
% %% Plots
% figure;
% plot(T, y_hist(1,:), T, y_hist(2,:), 'LineWidth',1.5); hold on;
% plot(T, r_const(1)*ones(1,N),'r--', T, r_const(2)*ones(1,N),'b--');
% grid on;
% legend('y1','y2','r1','r2');
% xlabel('Time [s]'); ylabel('Outputs');
% title('LQR + Observer Tracking for Constant Reference');
% 
% figure;
% plot(T, u_hist, 'LineWidth',1.5); grid on;
% legend('u1','u2'); xlabel('Time [s]'); ylabel('Inputs');
% title('Control Inputs');
% 
% figure;
% plot(T, x_hist(1,:), T, xhat_hist(1,:)); grid on;
% xlabel('Time [s]'); ylabel('|x - xhat|');
% title('Observer Estimation Error Norm');