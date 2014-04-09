function plot_all(prefix_s, prefix_d, show_conf)

% PLOT_ALL Plots all results in current folder
%     To be used with files './stability_output_prefix*'
%
%     PLOT_STABILITY(prefix_s, prefix_d, show_conf)
% 
%     prefix_s: stability results are stored in files 'prefix_s'. DON'T
%     forget to put the wildcard character '*'
% 
%     prefix_d: deficit results are stored in files 'prefix_d'. DON'T
%     forget to put the wildcard character '*'
% 
%     show_conf: =0 if network configuration is not to be displayed

list_s = dir(prefix_s);
for i=1:size(list_s,1)
    [pathstr,name,ext] = fileparts(list_s(i).name);
    if strcmp( ext,'.txt' );
        plot_stability(list_s(i).name);
    end
end

list_d = dir(prefix_d);
for j=1:size(list_d,1)
    [pathstr,name,ext] = fileparts(list_d(j).name);
    if strcmp( ext,'.txt' );
        plot_deficits(show_conf,list_d(j).name);
    end
end
