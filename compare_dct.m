close all;
load('patches/inputs.csv')
load('patches/dct.csv')
load ('patches/posPipe.csv')
load ('patches/full.csv')

% inshow(squeeze(uint8(input_patchs(1,:,:)*16)));
foo=inputs(30,:);
foob=reshape(foo,[48 48]);
% figure(1);
% imshow(uint8(foob));
figure(1);
imshow(uint8(foob*255));

% dct_patch=reshape(dct, [280 48 48]);
% actual_dct=dct_patch(1,:,:)-dct_patch(2,:,:);
% figure(2);
% imshow(actual_dct);
% actual_dct_c = squeeze(actual_dct(1,:,:));
% surf(squeeze(actual_dct));

% input_patchs=reshape(inputs,[140 48 48]);
% actual_dct_m=dct2(squeeze(input_patchs(1,:,:)));
% figure(2);
% surf(squeeze(actual_dct_m));

% figure(3);
% a = reshape(posPipe, [140 48 48]);
% b = a(1,:,:);
% % imshow((idct2(squeeze(b)))/6);
% surf(idct2(squeeze(b)));
% 
% figure(4);
% surf(idct2(squeeze(actual_dct_m)));

% figure(5);
% idct_patch=reshape(full, [560 48 48]);
% actual_idct = idct_patch(1,:,:)-idct_patch(2,:,:)+idct_patch(3,:,:)-idct_patch(4,:,:);
% imshow(squeeze(actual_idct));
% surf(squeeze(actual_idct));


