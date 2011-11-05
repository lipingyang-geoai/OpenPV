function [pvp_image] = pvp_drawBoundingBox(pvp_image, hit_list)

  num_hits = length(hit_list);
  for i_hit = 1 : num_hits

    %% draw patch boundaries in green
    pvp_image(hit_list{i_hit}.patch_Y1, ...
	      hit_list{i_hit}.patch_X1:hit_list{i_hit}.patch_X2, ...
	      2) = 255;
    pvp_image(hit_list{i_hit}.patch_Y2:hit_list{i_hit}.patch_Y3, ...
	      hit_list{i_hit}.patch_X2, ...
	      2) = 255;
    pvp_image(hit_list{i_hit}.patch_Y3, ...
	      hit_list{i_hit}.patch_X1:hit_list{i_hit}.patch_X2, ...
	      2) = 255;
    pvp_image(hit_list{i_hit}.patch_Y1:hit_list{i_hit}.patch_Y4, ...
	      hit_list{i_hit}.patch_X1, ...
	      2) = 255;
 
   pvp_image(hit_list{i_hit}.patch_Y1, ...
	      hit_list{i_hit}.patch_X1:hit_list{i_hit}.patch_X2, ...
	      1) = 0;
    pvp_image(hit_list{i_hit}.patch_Y2:hit_list{i_hit}.patch_Y3, ...
	      hit_list{i_hit}.patch_X2, ...
	      1) = 0;
    pvp_image(hit_list{i_hit}.patch_Y3, ...
	      hit_list{i_hit}.patch_X1:hit_list{i_hit}.patch_X2, ...
	      1) = 0;
    pvp_image(hit_list{i_hit}.patch_Y1:hit_list{i_hit}.patch_Y4, ...
	      hit_list{i_hit}.patch_X1, ...
	      1) = 0;
    pvp_image(hit_list{i_hit}.patch_Y1, ...
	      hit_list{i_hit}.patch_X1:hit_list{i_hit}.patch_X2, ...
	      3) = 0;
    pvp_image(hit_list{i_hit}.patch_Y2:hit_list{i_hit}.patch_Y3, ...
	      hit_list{i_hit}.patch_X2, ...
	      3) = 0;
    pvp_image(hit_list{i_hit}.patch_Y3, ...
	      hit_list{i_hit}.patch_X1:hit_list{i_hit}.patch_X2, ...
	      3) = 0;
    pvp_image(hit_list{i_hit}.patch_Y1:hit_list{i_hit}.patch_Y4, ...
	      hit_list{i_hit}.patch_X1, ...
	      3) = 0;

    %% draw bounding box in blue
    pvp_image(hit_list{i_hit}.BoundingBox_Y1, ...
	      hit_list{i_hit}.BoundingBox_X1:hit_list{i_hit}.BoundingBox_X2, ...
	      3) = 255;
    pvp_image(hit_list{i_hit}.BoundingBox_Y2:hit_list{i_hit}.BoundingBox_Y3, ...
	      hit_list{i_hit}.BoundingBox_X2, ...
	      3) = 255;
    pvp_image(hit_list{i_hit}.BoundingBox_Y3, ...
	      hit_list{i_hit}.BoundingBox_X1:hit_list{i_hit}.BoundingBox_X2, ...
	      3) = 255;
    pvp_image(hit_list{i_hit}.BoundingBox_Y1:hit_list{i_hit}.BoundingBox_Y4, ...
	      hit_list{i_hit}.BoundingBox_X1, ...
	      3) = 255;

    pvp_image(hit_list{i_hit}.BoundingBox_Y1, ...
	      hit_list{i_hit}.BoundingBox_X1:hit_list{i_hit}.BoundingBox_X2, ...
	      1) = 0;
    pvp_image(hit_list{i_hit}.BoundingBox_Y2:hit_list{i_hit}.BoundingBox_Y3, ...
	      hit_list{i_hit}.BoundingBox_X2, ...
	      1) = 0;
    pvp_image(hit_list{i_hit}.BoundingBox_Y3, ...
	      hit_list{i_hit}.BoundingBox_X1:hit_list{i_hit}.BoundingBox_X2, ...
	      1) = 0;
    pvp_image(hit_list{i_hit}.BoundingBox_Y1:hit_list{i_hit}.BoundingBox_Y4, ...
	      hit_list{i_hit}.BoundingBox_X1, ...
	      1) = 0;
    pvp_image(hit_list{i_hit}.BoundingBox_Y1, ...
	      hit_list{i_hit}.BoundingBox_X1:hit_list{i_hit}.BoundingBox_X2, ...
	      2) = 0;
    pvp_image(hit_list{i_hit}.BoundingBox_Y2:hit_list{i_hit}.BoundingBox_Y3, ...
	      hit_list{i_hit}.BoundingBox_X2, ...
	      2) = 0;
    pvp_image(hit_list{i_hit}.BoundingBox_Y3, ...
	      hit_list{i_hit}.BoundingBox_X1:hit_list{i_hit}.BoundingBox_X2, ...
	      2) = 0;
    pvp_image(hit_list{i_hit}.BoundingBox_Y1:hit_list{i_hit}.BoundingBox_Y4, ...
	      hit_list{i_hit}.BoundingBox_X1, ...
	      2) = 0;
  endfor

endfunction %% pvp_drawBoundingBox