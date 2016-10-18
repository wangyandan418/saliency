close all;

chan = 1;
sz_R = 2;
sz_C = 2;
img = im2double(imread('../images/000646.jpg'));
img(1,1:10,1)
% pause()
imggry = rgb2gray(img);
img2 = rgb2gray(im2double(imread('../images/000647.jpg')));
imagscaled = zeros(48*sz_R,48*sz_C,4);
imgscaled(:,:,1) = imresize(img(:,:,1),  [48*sz_R 48*sz_C]);
imgscaled(:,:,2) = imresize(img(:,:,2),  [48*sz_R 48*sz_C]);
imgscaled(:,:,3) = imresize(img(:,:,3),  [48*sz_R 48*sz_C]);
imgscaled(:,:,4) = imresize(imggry-img2, [48*sz_R 48*sz_C]);

combinedsal = zeros(48*sz_R,48*sz_C);

for chan = 1:1
sal = zeros(48*sz_R,48*sz_C);
sal_dct2 = zeros(48*sz_R,48*sz_C);
% for i = 0:(2*sz_R-2)
for i = 0:0    
%     for j = 0:(2*sz_C-2)
    for j = 0:0    
        patch = imgscaled(i*24+1:i*24+48,j*24+1:j*24+48,chan);
        
        figure(1);
        imshow(patch);
%         patch = patch ./ 16;
        salpatch_dct2 = dct2(patch);
%         surf(squeeze(salpatch_dct2));

        figure(2);
        imshow(salpatch_dct2);
        sal_dct2(i*24+1:i*24+48,j*24+1:j*24+48,1) = sal_dct2(i*24+1:i*24+48,j*24+1:j*24+48,1) + (salpatch_dct2);
%         salpatch = idct2((sign(dct2(patch))));
        salpatch = idct2(((dct2(patch))));
        
        figure(3);
        imshow(salpatch);
%         surf(squeeze(salpatch));
        sal(i*24+1:i*24+48,j*24+1:j*24+48,1) = sal(i*24+1:i*24+48,j*24+1:j*24+48,1) + (salpatch);
    end;
end;
combinedsal = combinedsal + sal;
end;

% for chan = 1:4
% sal = zeros(48*sz_R,48*sz_C);
% for i = 0:35
%         load('inputs_5m.csv');
%         patch = inputs_5m(1,:);        
%         salpatch = dct2(patch);
%         sal(i*24+1:i*24+48,j*24+1:j*24+48,1) = sal(i*24+1:i*24+48,j*24+1:j*24+48,1) + (salpatch);
   
% end;
% combinedsal = combinedsal + sal;
% end;

%imshow(combinedsal)
% imshow(imgaussfilt(combinedsal))