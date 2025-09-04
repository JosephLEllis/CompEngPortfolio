goodImage = imread('D:/PYNQ_CUSTOM_IPS_VIVADO_2019/Canny/Canny/Testbenches/line.jpg');

grayImage = im2gray(goodImage);

% Get image size
[M, N] = size(grayImage);

noisyImage = imnoise(grayImage, 'gaussian', 0, 1);

% Difference 1 = good image - bad image (no filter on bad image)
% Difference 2 = good image - bad image (filter on bad image)

% Box filter 
box = fspecial('average', [3 3]);

% Apply the Gaussian filter (convolution)
filteredImage = imgaussfilt(grayImage, 1, 'FilterSize', 3);

% Apply the box filter (convolution) 
boxFilterImage = imfilter(grayImage, box);

noFilter = grayImage - noisyImage; 

withFilter = grayImage - filteredImage; 

withBoxFilter = grayImage - boxFilterImage; 

% Find MSE for non-filtered noise image 
squareDiffNoFilt = noFilter.^2;
MSENoFilt = mean(squareDiffNoFilt, "all");
PSNRNoFilt = 10 * log10(255/MSENoFilt);

disp("MSE no filter: " + MSENoFilt);
disp("PSNR no filter: " + PSNRNoFilt);

% Find MSE for filtered noise image 
squareDiffFilt = withFilter.^2;
MSEFilt = mean(squareDiffFilt, "all");
PSNRFilt = 10 * log10(255/MSEFilt);

% Find MSE for filtered noise image box filter 
squareDiffBox = withBoxFilter.^2; 
MSEBox = mean(squareDiffBox, "all");
PSNRBox = 10 * log10(255/MSEBox);

disp("MSE with box filter: " + MSEBox);
disp("PSNR with box filter: " + PSNRBox); 

disp("MSE with filter: " + MSEFilt);
disp("PSNR with filter: " + PSNRFilt);

% Find Sobel for both 
sobelX = fspecial('sobel');
sobelY = sobelX';

gradX = imfilter(double(filteredImage), sobelX, 'replicate');
gradY = imfilter(double(filteredImage), sobelY, 'replicate');

magGrad = sqrt(gradX.^2 + gradY.^2);

% Perform 2D FFT on both images 

% Good image FFT
fftGrayImage = fft2(grayImage);

% No gaussian filter applied
fftNoFilter = fft2(noFilter);

% Gaussian filter applied 
fftWithFilter = fft2(withFilter);

% Box filter applied
fftWithBox = fft2(withBoxFilter);

Fshift = fftshift(fftGrayImage); 
FshiftNoFilter = fftshift(fftNoFilter);
FShiftWFilter = fftshift(fftWithFilter);
FShiftWBoxFilter = fftshift(fftWithBox);

pwr = abs(Fshift).^2; 

pwrNoFilter = abs(FshiftNoFilter).^2;

pwrWFilter = abs(FShiftWFilter).^2;

pwrWBoxFilter = abs(FShiftWBoxFilter).^2; 

% Create frequency index grid
[u, v] = meshgrid(-floor(N/2):ceil(N/2)-1, -floor(M/2):ceil(M/2)-1);

% Calculate normalized spatial frequency
lambda = sqrt((N^2 * v.^2 + M^2 * u.^2)/(M^2*N^2));

% Define band edges 
bandEdges = [0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8];

% Arrays to hold the band power values 
avgBandPowers = zeros(1, length(bandEdges)-1);
avgBandPwrDiffNoFilter = zeros(1, length(bandEdges)-1);
avgBandPwrDiffWFilter = zeros(1, length(bandEdges)-1);
avgBandPwrDiffWBoxFilter = zeros(1, length(bandEdges)-1);

% Avg pwr for normal image
for k = 1:length(bandEdges)-1
    mask = lambda >= bandEdges(k) & lambda < bandEdges(k+1);
    bandValues = pwr(mask);
    avgBandPowers(k) = mean(bandValues);  
end

% Avg pwr for no filter noise image
for k = 1:length(bandEdges)-1
    mask = lambda >= bandEdges(k) & lambda < bandEdges(k+1);
    bandValues = pwrNoFilter(mask);
    avgBandPwrDiffNoFilter(k) = mean(bandValues);  
end

% Avg pwr for filter with noise image
for k = 1:length(bandEdges)-1
    mask = lambda >= bandEdges(k) & lambda < bandEdges(k+1);
    bandValues = pwrWFilter(mask);
    avgBandPwrDiffWFilter(k) = mean(bandValues);  
end

% Avg pwr for box filter with noise image
for k = 1:length(bandEdges)-1
    mask = lambda >= bandEdges(k) & lambda < bandEdges(k+1);
    bandValues = pwrWBoxFilter(mask);
    avgBandPwrDiffWBoxFilter(k) = mean(bandValues);  
end

% Convert avgBandPowers to dB
avgBandPowers = 10 * log10(avgBandPowers);
avgBandPwrDiffWFilter = 10 * log10(avgBandPwrDiffWFilter);
avgBandPwrDiffNoFilter = 10 * log10(avgBandPwrDiffNoFilter);
avgBandPwrDiffWBoxFilter = 10 * log10(avgBandPwrDiffWBoxFilter);

% Find SNR for no filter
SNRNoFilter = zeros(1, length(bandEdges)-1);
SNRWFilter = zeros(1, length(bandEdges)-1);
SNRWBoxFilter = zeros(1, length(bandEdges)-1);

for k = 1:length(bandEdges)-1
    SNRNoFilter(k) = (avgBandPowers(k) - avgBandPwrDiffNoFilter(k));
end

for k = 1:length(bandEdges)-1
    SNRWFilter(k) = (avgBandPowers(k) - avgBandPwrDiffWFilter(k));
end

for k = 1:length(bandEdges)-1
    SNRWBoxFilter(k) = (avgBandPowers(k) - avgBandPwrDiffWBoxFilter(k));
end

figure(1);
imagesc(u(1,:), v(:,1), log(1 + pwr));
xlabel('u (horizontal frequency index)');
ylabel('v (vertical frequency index)');
title('Magnitude Spectrum with Frequency Axes');
axis image;
colormap('gray'); colorbar;

hold on;
contour(u, v, lambda, [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7], 'LineColor', 'w');
hold off;

figure(2);

xNoFilter = bandEdges(1:end-1);
xWFilter  = bandEdges(1:end-1);
xWBoxFilter = bandEdges(1:end-1);

plot(xNoFilter, SNRNoFilter, '-o', 'LineWidth',2); hold on;
plot(xWBoxFilter, SNRWBoxFilter, '-s', 'LineWidth', 2); hold on;
plot(xWFilter,  SNRWFilter,  '-s', 'LineWidth',2); hold off;

set(gca, 'XTick', bandEdges);

xlim([bandEdges(1), bandEdges(end-1)]);
xlabel('Normalized spatial frequency (\lambda)');
ylabel('SNR (dB)');
legend('No Filter','With Box filter', 'With Gaussian filter','Location','Best');
grid on;
title('Per‐Band SNR vs. Normalized spatial frequency');