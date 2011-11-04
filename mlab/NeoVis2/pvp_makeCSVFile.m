
function [tot_frames, ...
	  tot_time, ...
	  CSV_struct] = ...
      pvp_makeCSVFile(NEOVISION_DATASET_ID, ...
		      NEOVISION_DISTRIBUTION_ID, ...
		      CSV_path, ...
		      neovision_dataset_ID, ...
		      pvp_frame_offset, ...
		      pvp_frame_skip, ...
		      ObjectType, ...
		      chip_path, ...
		      patch_size, ...
		      pvp_path, ...
		      pvp_layer, ...
		      training_flag, ...
		      num_procs)
  %% takes PetaVision non-spiking activity files generated in response to
  %% a video clip and produces a CSV file indicating locations of
  %% specified object
  %%keyboard;
  begin_time = time();
  
  machine_path = ...
      [filesep, "Users", filesep, "gkenyon", filesep];
  
  num_input_args = 0;
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("NEOVISION_DATASET_ID") || isempty(NEOVISION_DATASET_ID)
    NEOVISION_DATASET_ID = "Tower";
  endif
  neovision_dataset_id = tolower(NEOVISION_DATASET_ID); %% 
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("NEOVISION_DISTRIBUTION_ID") || isempty(NEOVISION_DISTRIBUTION_ID)
    NEOVISION_DISTRIBUTION_ID = "Formative";
  endif
  neovision_distribution_id = tolower(NEOVISION_DISTRIBUTION_ID); %% 
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("CSV_path") || isempty(CSV_path)
    CSV_path = [machine_path, "Pictures", filesep, "NeoVision", filesep, NEOVISION_DATASET_ID, filesep, ...
		"neovision-data-", neovision_distribution_id, "-", neovision_dataset_id, filesep]; %% 
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("clip_name") || isempty(clip_name)
    clip_name = "050";
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("pvp_frame_skip") || isempty(pvp_frame_skip)
    pvp_frame_skip = 1;
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("pvp_frame_skip") || isempty(pvp_frame_skip)
    pvp_frame_offset = 1;
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("ObjectType") || isempty(ObjectType)
    ObjectType = "Cyclist";
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("chip_path") || isempty(chip_path)
    chip_path = [machine_path, "Pictures", filesep, "NeoVision", filesep, NEOVISION_DATASET_ID, filesep, ...
		 "neovision-chips-", neovision_dataset_id, filesep]; %% 
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("patch_size") || isempty(patch_size)
    chip_log_dir = [chip_path, "log", filesep, ObjectType, filesep];
    chip_log_pathname = [chip_log_dir, "log.txt"];
    if exist(chip_log_pathname, "file")
      chip_log_struct = struct;
      chip_log_fieldnames = ...
	  { ...
	   "tot_unread", ...
	   "tot_rejected", ...
	   "tot_chips", ...
	   "tot_DoG", ...
	   "tot_canny", ...
	   "tot_cropped", ...
	   "tot_mean", ...
	   "tot_std", ...
	   "tot_border_artifact_top", ...
	   "tot_border_artifact_bottom", ...
	   "tot_border_artifact_left", ...
	   "tot_border_artifact_right", ...
	   "ave_original_size", ...
	   "ave_cropped_size", ...
	   "std_original_size", ...
	   "std_cropped_size" };
      chip_log_fid = fopen(chip_log_pathname, "r");
      chip_log_struct.tot_unread = str2num(fgets(chip_log_fid));
      chip_log_struct.tot_rejected = str2num(fgets(chip_log_fid));
      chip_log_struct.tot_chips = str2num(fgets(chip_log_fid));
      chip_log_struct.tot_DoG = str2num(fgets(chip_log_fid));
      chip_log_struct.tot_canny = str2num(fgets(chip_log_fid));
      chip_log_struct.tot_cropped = str2num(fgets(chip_log_fid));
      chip_log_struct.tot_mean = str2num(fgets(chip_log_fid));
      chip_log_struct.tot_std = str2num(fgets(chip_log_fid));
      chip_log_struct.tot_border_artifact_top = str2num(fgets(chip_log_fid));
      chip_log_struct.tot_border_artifact_bottom = str2num(fgets(chip_log_fid));
      chip_log_struct.tot_border_artifact_left = str2num(fgets(chip_log_fid));
      chip_log_struct.tot_border_artifact_right = str2num(fgets(chip_log_fid));
      chip_log_struct.ave_original_size = str2num(fgets(chip_log_fid));
      chip_log_struct.ave_cropped_size = str2num(fgets(chip_log_fid));
      chip_log_struct.std_original_size = str2num(fgets(chip_log_fid));
      chip_log_struct.std_cropped_size = str2num(fgets(chip_log_fid));
      fclose(chip_log_fid);
      patch_size = ...
	  fix(chip_log_struct.ave_original_size + chip_log_struct.std_original_size);
    else
      patch_size = [128, 128];
    endif %% exist(chip_log_pathname)
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("pvp_path") || isempty(pvp_path)
    pvp_path = [machine_path, "workspace-indigo", filesep, "Clique2", ...
		filesep, "input", filesep, "Tower", filesep, clip_name, filesep, ...
		ObjectType, "2", filesep, "DoG", filesep];
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("pvp_layer") || isempty(pvp_layer)
    pvp_layer = 5;  %% 
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("pvp_training_flag") || isempty(pvp_training_flag)
    training_flag = 1;
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("num_procs") || isempty(num_procs)
    num_procs = 8;  %% 
  endif
  
  global pvp_patch_size
  pvp_patch_size = patch_size;
  
  global pvp_training_flag
  pvp_training_flag = training_flag;
  
  object_list = cell(12,1);
  object_list{1} = "Boat"; 
  object_list{2} = "Bus"; 
  object_list{3} = "Car"; 
  object_list{4} = "Container"; 
  object_list{5} = "Cyclist"; 
  object_list{6} = "Helicopter"; 
  object_list{7} = "Person"; 
  object_list{8} = "Plane"; 
  object_list{9} = "Tractor-Trailer"; 
  object_list{10} = "Truck"; 
  object_list{11} = "distractor";  %% non-DARPA object
  object_list{12} = "target"; %% any DARPA object  
  
  %%setenv('GNUTERM', 'x11');
  image_type = ".png";
  
  %% path to generic image processing routines
  img_proc_dir = "~/workspace-indigo/PetaVision/mlab/imgProc/";
  addpath(img_proc_dir);
  
  %% path to string manipulation kernels for use with parcellfun
  str_kernel_dir = "~/workspace-indigo/PetaVision/mlab/stringKernels/";
  addpath(str_kernel_dir);
  
  global ODD_dir
  ODD_path = [CSV_path, "ODD", filesep]; 
  mkdir(ODD_path);
  ODD_clip_dir = [ODD_path, clip_name, filesep];
  mkdir(ODD_clip_dir);
  ODD_dir = [ODD_clip_dir, ObjectType, filesep];
  mkdir(ODD_dir);
  
  ROC_path = [CSV_path, "ROC", filesep]; 
  mkdir(ROC_path);
  ROC_clip_dir = [ROC_path, clip_name, filesep];
  mkdir(ROC_clip_dir);
  ROC_dir = [ROC_clip_dir, ObjectType, filesep];
  mkdir(ROC_dir);
  
  global pvp_density_thresh
  BB_stats_pathname = [ROC_dir, "BB_stats.txt"];
  if exist(BB_stats_pathname, "file")
    BB_stats_struct = struct;
    BB_stats_fid = fopen(BB_stats_pathname, "r");
    BB_stats_struct.pvp_min_BB_density = str2num(fgets(BB_stats_fid));
    BB_stats_struct.pvp_max_BB_density = str2num(fgets(BB_stats_fid));
    BB_stats_struct.pvp_ave_BB_density = str2num(fgets(BB_stats_fid));
    BB_stats_struct.pvp_std_BB_density = str2num(fgets(BB_stats_fid));
    if ~feof(BB_stats_fid)
      BB_stats_struct.pvp_median_BB_density = str2num(fgets(BB_stats_fid));
    endif
    fclose(BB_stats_fid);
    pvp_density_thresh = ...
	(BB_stats_struct.pvp_ave_BB_density(1)); %% + BB_stats_struct.pvp_std_BB_density(1)) / 2;
  else
    pvp_density_thresh = -1.0;  %% flag to use ave density across image
  endif
  disp(["pvp_density_thresh = ", num2str(pvp_density_thresh)]);
  
  
  true_path = [CSV_path, "CSV", filesep];
  true_CSV_filename = [clip_name, ".csv"];
  true_CSV_pathname = [true_path, true_CSV_filename];
  true_CSV_fid = fopen(true_CSV_pathname, "r");
  true_CSV_header = fgets(true_CSV_fid);
  true_CSV_list = cell(1);
  i_CSV = 0;
  while ~feof(true_CSV_fid)
    i_CSV = i_CSV + 1;
    true_CSV_list{i_CSV} = fgets(true_CSV_fid);
  endwhile
  fclose(true_CSV_fid);
  num_true_CSV = i_CSV;
  
  %% get frame IDs 
  clip_dir = [CSV_path, "clips", filesep, clip_name, filesep];
  if ~exist(clip_dir, "dir")
    error(["~exist(clip_dir):", clip_dir]);
  endif
  frameIDs_path = ...
      [clip_dir, '*', image_type];
  frame_pathnames_all = glob(frameIDs_path);
  num_frames = size(frame_pathnames_all,1);
  disp(["num_frames = ", num2str(num_frames)]);
  
  tot_frames = length(pvp_frame_offset : pvp_frame_skip : num_frames);
  disp(["tot_frames = ", num2str(tot_frames)]);
  
  %% read pvp activity into cell array
  [pvp_fid, ...
   pvp_header, ...
   pvp_index ] = ...
      pvp_openActivityFile(pvp_path, pvp_layer);
  [layerID] = neoVisLayerID(pvp_layer);
  
  global NFEATURES NCOLS NROWS N
  NCOLS = pvp_header(pvp_index.NX_GLOBAL);
  NROWS = pvp_header(pvp_index.NY_GLOBAL);
  NFEATURES = pvp_header(pvp_index.NF);
  N = NFEATURES * NCOLS * NROWS;
  
  pvp_time = zeros(tot_frames, 1);
  pvp_offset = zeros(tot_frames, 1);
  pvp_time = cell(tot_frames, 1);
  pvp_activity = cell(tot_frames, 1);
  frame_pathnames = cell(tot_frames, 1);
  
  pvp_offset_tmp = 0;
  i_frame = 0;
  for j_frame = pvp_frame_offset : pvp_frame_skip : num_frames
    i_frame = i_frame + 1;
    pvp_frame = j_frame + pvp_layer - 2;
    [pvp_time{i_frame},...
     pvp_activity{i_frame}, ...
     pvp_offset(i_frame)] = ...
	pvp_readSparseLayerActivity(pvp_fid, pvp_frame, pvp_header, pvp_index, pvp_offset_tmp);
    if pvp_offset(i_frame) == -1
      break;
    endif
    pvp_offset_tmp = pvp_offset(i_frame);
    frame_pathnames{i_frame} = frame_pathnames_all{j_frame};
    disp(["i_frame = ", num2str(i_frame)]);
    disp(["pvp_time = ", num2str(pvp_time{i_frame})]);
    disp(["frame_ID = ", frame_pathnames{i_frame}]);
    disp(["mean(pvp_activty) = ", num2str(mean(pvp_activity{i_frame}(:)))]);    
  endfor
  fclose(pvp_fid);
  nnz_frames = i_frame-1;
  disp(["nnz_frames = ", num2str(nnz_frames)]);
  
  %% struct for storing rank order of comma separators between fields
  true_CSV_struct = cell(tot_frames, 1);
  if pvp_training_flag
    true_CSV_comma_rank = struct;
    true_CSV_comma_rank.Frame = [1, 2];
    true_CSV_comma_rank.BoundingBox_X1 = [2, 3];
    true_CSV_comma_rank.BoundingBox_Y1 = [3, 4];
    true_CSV_comma_rank.BoundingBox_X2 = [4, 5];
    true_CSV_comma_rank.BoundingBox_Y2 = [5, 6];
    true_CSV_comma_rank.BoundingBox_X3 = [6, 7];
    true_CSV_comma_rank.BoundingBox_Y3 = [7, 8];
    true_CSV_comma_rank.BoundingBox_X4 = [8, 9];
    true_CSV_comma_rank.BoundingBox_Y4 = [9, 10];
    true_CSV_comma_rank.ObjectType = [10, 11];
    
    for i_CSV = 1 : num_true_CSV
      true_CSV_comma_ndx = [1, strfind(true_CSV_list{i_CSV}, ",")];
      ObjectType_ndx(1) = true_CSV_comma_ndx(true_CSV_comma_rank.ObjectType(1))+1;
      ObjectType_ndx(2) = true_CSV_comma_ndx(true_CSV_comma_rank.ObjectType(2))-1;
      CSV_ObjectType = true_CSV_list{i_CSV}(ObjectType_ndx(1):ObjectType_ndx(2));
      if ~strcmp(CSV_ObjectType, ObjectType)
	continue;
      endif
      true_CSV_struct_tmp.ObjectType = CSV_ObjectType;
      Frame_ndx(1) = true_CSV_comma_ndx(true_CSV_comma_rank.Frame(1));
      Frame_ndx(2) = true_CSV_comma_ndx(true_CSV_comma_rank.Frame(2))-1;
      true_Frame = true_CSV_list{i_CSV}(Frame_ndx(1):Frame_ndx(2));
      i_frame = str2num(true_Frame) + 1;
      if i_frame > nnz_frames
	break;
      endif
      true_CSV_struct_tmp = struct;
      true_CSV_struct_tmp.Frame = true_Frame;
      BoundingBox_X1_ndx(1) = true_CSV_comma_ndx(true_CSV_comma_rank.BoundingBox_X1(1))+1;
      BoundingBox_X1_ndx(2) = true_CSV_comma_ndx(true_CSV_comma_rank.BoundingBox_X1(2))-1;
      BoundingBox_X1 = true_CSV_list{i_CSV}(BoundingBox_X1_ndx(1):BoundingBox_X1_ndx(2));
      true_CSV_struct_tmp.BoundingBox_X1 = str2num(BoundingBox_X1);
      BoundingBox_Y1_ndx(1) = true_CSV_comma_ndx(true_CSV_comma_rank.BoundingBox_Y1(1))+1;
      BoundingBox_Y1_ndx(2) = true_CSV_comma_ndx(true_CSV_comma_rank.BoundingBox_Y1(2))-1;
      BoundingBox_Y1 = true_CSV_list{i_CSV}(BoundingBox_Y1_ndx(1):BoundingBox_Y1_ndx(2));
      true_CSV_struct_tmp.BoundingBox_Y1 = str2num(BoundingBox_Y1);
      BoundingBox_X2_ndx(1) = true_CSV_comma_ndx(true_CSV_comma_rank.BoundingBox_X2(1))+1;
      BoundingBox_X2_ndx(2) = true_CSV_comma_ndx(true_CSV_comma_rank.BoundingBox_X2(2))-1;
      BoundingBox_X2 = true_CSV_list{i_CSV}(BoundingBox_X2_ndx(1):BoundingBox_X2_ndx(2));
      true_CSV_struct_tmp.BoundingBox_X2 = str2num(BoundingBox_X2);
      BoundingBox_Y2_ndx(1) = true_CSV_comma_ndx(true_CSV_comma_rank.BoundingBox_Y2(1))+1;
      BoundingBox_Y2_ndx(2) = true_CSV_comma_ndx(true_CSV_comma_rank.BoundingBox_Y2(2))-1;
      BoundingBox_Y2 = true_CSV_list{i_CSV}(BoundingBox_Y2_ndx(1):BoundingBox_Y2_ndx(2));
      true_CSV_struct_tmp.BoundingBox_Y2 = str2num(BoundingBox_Y2);
      BoundingBox_X3_ndx(1) = true_CSV_comma_ndx(true_CSV_comma_rank.BoundingBox_X3(1))+1;
      BoundingBox_X3_ndx(2) = true_CSV_comma_ndx(true_CSV_comma_rank.BoundingBox_X3(2))-1;
      BoundingBox_X3 = true_CSV_list{i_CSV}(BoundingBox_X3_ndx(1):BoundingBox_X3_ndx(2));
      true_CSV_struct_tmp.BoundingBox_X3 = str2num(BoundingBox_X3);
      BoundingBox_Y3_ndx(1) = true_CSV_comma_ndx(true_CSV_comma_rank.BoundingBox_Y3(1))+1;
      BoundingBox_Y3_ndx(2) = true_CSV_comma_ndx(true_CSV_comma_rank.BoundingBox_Y3(2))-1;
      BoundingBox_Y3 = true_CSV_list{i_CSV}(BoundingBox_Y3_ndx(1):BoundingBox_Y3_ndx(2));
      true_CSV_struct_tmp.BoundingBox_Y3 = str2num(BoundingBox_Y3);
      BoundingBox_X4_ndx(1) = true_CSV_comma_ndx(true_CSV_comma_rank.BoundingBox_X4(1))+1;
      BoundingBox_X4_ndx(2) = true_CSV_comma_ndx(true_CSV_comma_rank.BoundingBox_X4(2))-1;
      BoundingBox_X4 = true_CSV_list{i_CSV}(BoundingBox_X4_ndx(1):BoundingBox_X4_ndx(2));
      true_CSV_struct_tmp.BoundingBox_X4 = str2num(BoundingBox_X4);
      BoundingBox_Y4_ndx(1) = true_CSV_comma_ndx(true_CSV_comma_rank.BoundingBox_Y4(1))+1;
      BoundingBox_Y4_ndx(2) = true_CSV_comma_ndx(true_CSV_comma_rank.BoundingBox_Y4(2))-1;
      BoundingBox_Y4 = true_CSV_list{i_CSV}(BoundingBox_Y4_ndx(1):BoundingBox_Y4_ndx(2));
      true_CSV_struct_tmp.BoundingBox_Y4 = str2num(BoundingBox_Y4);
      num_BBs = length(true_CSV_struct{i_frame});
      true_CSV_struct{i_frame}{num_BBs + 1} = true_CSV_struct_tmp;
    endfor
  endif %% pvp_training_flag
  
  disp("");
  if num_procs > 1
    CSV_struct = parcellfun(num_procs, @pvp_makeCSVFileKernel, ...
			    frame_pathnames, pvp_time, pvp_activity, true_CSV_struct, ...
			    "UniformOutput", false);
  else
    CSV_struct = cellfun(@pvp_makeCSVFileKernel, ...
			 frame_pathnames, pvp_time, pvp_activity, true_CSV_struct, ...
			 "UniformOutput", false);
  endif
  
  disp("");

  pvp_results_path = [CSV_path, "results", filesep];
  mkdir(pvp_results_path);
  pvp_results_dir = [pvp_results_path, clip_name, filesep];
  mkdir(pvp_results_dir);
  frames_per_CSV_file = 150;
  num_CSV_files = ceil(nnz_frames / frames_per_CSV_file);
  CSV_ObjectType = ObjectType;
  CSV_Occlusion = 0; %% false
  CSV_Ambiguous = 0; %% false
  CSV_SiteInfo = 0;
  CSV_Version = 1.4;
  pvp_tot_hits = 0;
  pvp_tot_miss = 0;
  pvp_miss_density = [];
  pvp_hit_density = [];
  for i_CSV_file = 1 : num_CSV_files
    pvp_results_filename = ...
	[NEOVISION_DATASET_ID, "_", clip_name, num2st(i_CSV_file-1, "%3.3i"),...
	 "_PetaVision_", ObjectType, ".csv"];
    pvp_results_pathname = [pvp_results_dir, pvp_results_filename];
    pvp_results_fid = fopen(pvp_results_pathname, "w");
    fputs(pvp_results_fid, true_CSV_header);
    start_frame = 1 + frames_per_CSV_file * (i_CSV_file - 1);
    stop_frame = frames_per_CSV_file * i_CSV_file;
    stop_frame = min(stop_frame, nnz_frames);
    for i_frame = start_frame : stop_frame
      CSV_struct{i_frame}.Frame = pvp_frame_offset + pvp_frame_skip*(i_frame-1) - 1;
      disp(["frame_ID = ", CSV_struct{i_frame}.frame_filename]);
      disp(["pvp_time = ", num2str(CSV_struct{i_frame}.pvp_time)]);
      disp(["mean(pvp_activty) = ", num2str(CSV_struct{i_frame}.mean_activity)]);    
      disp(["num_active = ", num2str(CSV_struct{i_frame}.num_active)]);
      if pvp_training_flag
	disp(["num_active_BB_mask = ", num2str(CSV_struct{i_frame}.num_active_BB_mask)]);
	disp(["num_active_BB_notmask = ", num2str(CSV_struct{i_frame}.num_active_BB_notmask)]);
	disp(["num_BB_mask = ", num2str(CSV_struct{i_frame}.num_BB_mask)]);
	disp(["num_BB_notmask = ", num2str(CSV_struct{i_frame}.num_BB_notmask)]);
      endif
      pvp_num_hits = length(CSV_struct{i_frame}.hit_list);
      pvp_tot_hits = pvp_tot_hits + pvp_num_hits;
      pvp_num_miss = numel(CSV_struct{i_frame}.miss_list) - pvp_num_hits;
      pvp_miss_density = [pvp_miss_density; CSV_struct{i_frame}.miss_list(:)];
      pvp_tot_miss = pvp_tot_miss + pvp_num_miss;
      for i_hit = 1 : pvp_num_hits
	pvp_hit_density = [pvp_hit_density; CSV_struct{i_frame}.hit_list{i_hit}.hit_density];
	csv_str = [];
	csv_str = num2str(CSV_struct{i_frame}.Frame);
	csv_str = [csv_str, ",", num2str(CSV_struct{i_frame}.hit_list{i_hit}.BoundingBox_X1)];
	csv_str = [csv_str, ",", num2str(CSV_struct{i_frame}.hit_list{i_hit}.BoundingBox_Y1)];
	csv_str = [csv_str, ",", num2str(CSV_struct{i_frame}.hit_list{i_hit}.BoundingBox_X2)];
	csv_str = [csv_str, ",", num2str(CSV_struct{i_frame}.hit_list{i_hit}.BoundingBox_Y2)];
	csv_str = [csv_str, ",", num2str(CSV_struct{i_frame}.hit_list{i_hit}.BoundingBox_X3)];
	csv_str = [csv_str, ",", num2str(CSV_struct{i_frame}.hit_list{i_hit}.BoundingBox_Y3)];
	csv_str = [csv_str, ",", num2str(CSV_struct{i_frame}.hit_list{i_hit}.BoundingBox_X4)];
	csv_str = [csv_str, ",", num2str(CSV_struct{i_frame}.hit_list{i_hit}.BoundingBox_Y4)];
	csv_str = [csv_str, ",", CSV_ObjectType];
	csv_str = [csv_str, ",", num2str(CSV_Occlusion)];
	csv_str = [csv_str, ",", num2str(CSV_Ambiguous)];
	csv_str = [csv_str, ",", num2str(CSV_struct{i_frame}.hit_list{i_hit}.Confidence)];
	csv_str = [csv_str, ",", num2str(CSV_SiteInfo)];
	csv_str = [csv_str, ",", num2str(CSV_Version)];
	csv_str = [csv_str, "\n"];
	fputs(pvp_results_fid, csv_str);
      endfor %% i_hit
      disp("");
    endfor %% i_frame
  endfor %% i_CSV_file
  fclose(pvp_results_fid);
  
  %%keyboard;
  disp(["tot_hits = ", num2str(pvp_tot_hits)]);
  disp(["tot_miss = ", num2str(pvp_tot_miss)]);
  pvp_num_hit_and_miss_bins = 100;
  pvp_min_hit_density = min(pvp_hit_density);
  pvp_max_hit_density = max(pvp_hit_density);
  pvp_min_miss_density = min(pvp_miss_density);
  pvp_max_miss_density = max(pvp_miss_density);
  disp(["min_hit_density = ", num2str(pvp_min_hit_density)]);
  disp(["max_hit_density = ", num2str(pvp_max_hit_density)]);
  disp(["min_miss_density = ", num2str(pvp_min_miss_density)]);
  disp(["max_miss_density = ", num2str(pvp_max_miss_density)]);
  pvp_ave_hit_density = sum(pvp_hit_density) / pvp_tot_hits;
  pvp_std_hit_density = sqrt(sum(pvp_hit_density.^2) / pvp_tot_hits);
  pvp_median_hit_density = median(pvp_hit_density);
  pvp_ave_miss_density = sum(pvp_miss_density) / pvp_tot_miss;
  pvp_std_miss_density = sqrt(sum(pvp_miss_density.^2) / pvp_tot_miss);
  pvp_median_miss_density = median(pvp_miss_density);
  disp(["ave_hit_density = ", num2str(pvp_ave_hit_density)]);
  disp(["std_hit_density = ", num2str(pvp_std_hit_density)]);
  disp(["median_hit_density = ", num2str(pvp_median_hit_density)]);
  disp(["ave_miss_density = ", num2str(pvp_ave_miss_density)]);
  disp(["std_miss_density = ", num2str(pvp_std_miss_density)]);
  disp(["median_miss_density = ", num2str(pvp_median_miss_density)]);
  pvp_hit_and_miss_stats_pathname = [ROC_dir, "hit_and_miss_stats.txt"];
  save("-ascii", ...
       pvp_hit_and_miss_stats_pathname, ...
       "pvp_tot_hits", ...
       "pvp_tot_miss", ...
       "pvp_min_hit_density", ...
       "pvp_max_hit_density", ...
       "pvp_ave_hit_density", ...
       "pvp_std_hit_density", ...
       "pvp_median_hit_density", ...
       "pvp_min_miss_density", ...
       "pvp_max_miss_density", ...
       "pvp_ave_miss_density", ...
       "pvp_std_miss_density", ...
       "pvp_median_miss_density");
  [pvp_hit_and_miss_hist, pvp_hit_and_miss_bins] = ...
      hist([pvp_miss_density; pvp_hit_density], pvp_num_hit_and_miss_bins);
  pvp_hit_hist = hist(pvp_hit_density, pvp_hit_and_miss_bins);
  pvp_miss_hist = hist(pvp_miss_density, pvp_hit_and_miss_bins);
  pvp_hit_and_miss_hist_fig = figure;
  pvp_hit_and_miss_bh = ...
      bar(pvp_hit_and_miss_bins(2:pvp_num_hit_and_miss_bins), ...
	  pvp_hit_hist(2:pvp_num_hit_and_miss_bins), 0.8);
  set( pvp_hit_and_miss_bh, 'EdgeColor', [1 0 0] );
  set( pvp_hit_and_miss_bh, 'FaceColor', [1 0 0] );
  hold on;
  pvp_hit_and_miss_bh = ...
      bar(pvp_hit_and_miss_bins(2:pvp_num_hit_and_miss_bins), ...
	  pvp_miss_hist(2:pvp_num_hit_and_miss_bins), 0.6);
  set( pvp_hit_and_miss_bh, 'EdgeColor', [0 0 1] );
  set( pvp_hit_and_miss_bh, 'FaceColor', [0 0 1] );
  pvp_hit_and_miss_hist_pathname = [ROC_dir, "hit_and_miss_hist.png"];
  print(pvp_hit_and_miss_hist_fig, pvp_hit_and_miss_hist_pathname);
  disp("");
  %%close all;
  
  
  
  if pvp_training_flag == 1
    pvp_BB_density = zeros(nnz_frames, 2);
    pvp_num_BB_hist_bins = 100;
    for i_frame = 1 : nnz_frames
      pvp_BB_density(i_frame, 1) = ...
	  CSV_struct{i_frame}.num_active_BB_mask / ...
	  (CSV_struct{i_frame}.num_BB_mask + (CSV_struct{i_frame}.num_BB_mask == 0));
      pvp_BB_density(i_frame, 2) = ...
	  CSV_struct{i_frame}.num_active_BB_notmask / ...
	  (CSV_struct{i_frame}.num_BB_notmask + (CSV_struct{i_frame}.num_BB_notmask == 0));
    endfor
    pvp_min_BB_density = min(pvp_BB_density);
    pvp_max_BB_density = max(pvp_BB_density);
    pvp_ave_BB_density = mean(pvp_BB_density);
    pvp_std_BB_density = std(pvp_BB_density);
    pvp_median_BB_density = median(pvp_BB_density);
    pvp_z_score = ...
	(pvp_ave_BB_density(1) - pvp_ave_BB_density(2)) / ...
	(pvp_std_BB_density(1) + pvp_std_BB_density(2));
    disp("");
    disp(["min_BB_density = ", num2str(pvp_min_BB_density)]);
    disp(["max_BB_density = ", num2str(pvp_max_BB_density)]);
    disp(["ave_BB_density = ", num2str(pvp_ave_BB_density)]);
    disp(["std_BB_density = ", num2str(pvp_std_BB_density)]);
    disp(["median_BB_density = ", num2str(pvp_median_BB_density)]);
    disp(["z_score = ", num2str(pvp_z_score)]);
    pvp_BB_hist_delta = ...
	(pvp_max_BB_density(1) - pvp_min_BB_density(2)) / ...
	pvp_num_BB_hist_bins;
    pvp_BB_hist_edges = pvp_min_BB_density(2) : pvp_BB_hist_delta : pvp_max_BB_density(1);
    pvp_BB_hist_centers = ...
	(pvp_BB_hist_edges(2:pvp_num_BB_hist_bins+1) + pvp_BB_hist_edges(1:pvp_num_BB_hist_bins)) / 2;
    pvp_BB_hist_centers = [pvp_BB_hist_centers, pvp_BB_hist_centers(end)+pvp_BB_hist_delta];
    pvp_BB_hist = ...
	histc(pvp_BB_density, pvp_BB_hist_edges);
    pvp_BB_hist_fig = figure;
    pvp_bh = bar(pvp_BB_hist_centers, pvp_BB_hist(:,1), 0.8);
    hold on;
    set( pvp_bh, 'EdgeColor', [1 0 0] );
    set( pvp_bh, 'FaceColor', [1 0 0] );
    pvp_bh = bar(pvp_BB_hist_centers, pvp_BB_hist(:,2), 0.6);
    set( pvp_bh, 'EdgeColor', [0 0 1] );
    set( pvp_bh, 'FaceColor', [0 0 1] );
    pvp_BB_hist_pathname = [ROC_dir, "BB_hist.png"];
    print(pvp_BB_hist_fig, pvp_BB_hist_pathname, "-dpng");
    pvp_BB_stats_pathname = [ROC_dir, "BB_stats.txt"];
    save("-ascii", ...
	 pvp_BB_stats_pathname, ...
	 "pvp_min_BB_density", ...
	 "pvp_max_BB_density", ...
	 "pvp_ave_BB_density", ...
	 "pvp_std_BB_density", ...
	 "pvp_median_BB_density");
    disp("");
  endif
  

  end_time = time();
  tot_time = end_time - begin_time;
  disp(["tot_time = ", num2str(tot_time)]);

  

endfunction %% pvp_makeCSVFile



