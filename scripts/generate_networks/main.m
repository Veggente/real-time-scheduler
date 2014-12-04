% Generate unit-disk random graphs for C++ program
% real-time-scheduler
%
% The file network-1.txt is generated with the following parameters
%     num_nodes = 32;
%     comm_range = 0.25;
%     interf_range = 0.01;
%     max_num_links = 20;
% The file network-5.txt is generated with the following parameters
%     num_nodes = 32;
%     comm_range = 0.25;
%     interf_range = 0.21;
%     max_num_links = 20;

clear all; clc

rng(3)
num_nodes = 32;
comm_range = 0.25;
interf_range = 0.21;
max_num_links = 20;
plot_indicator = true;
link_mark_indicator = true;

[num_links, edges] = unitdiskrandomgraph(num_nodes, comm_range,...
    interf_range, max_num_links, plot_indicator, link_mark_indicator);
neighbors = getneighbors(num_links, edges);
max_neighbors = getmaxneighbors(num_links, neighbors);
maximal_schedule_matrix = getmaxschedules(1:num_links, neighbors);
output_filename = 'network-5.txt';
fid = fopen(output_filename, 'w');
fprintf(fid, 'Number of links: %d\n', num_links);
num_max_schedules = size(maximal_schedule_matrix, 2);
fprintf(fid, 'Number of maximal schedules: %d\n', num_max_schedules);
fprintf(fid, 'Maximal schedules:\n');
max_independent_set_size = 0;
for I = 1:num_max_schedules
    current_independent_set_size = length(maximal_schedule_matrix{I});
    if current_independent_set_size > max_independent_set_size
        max_independent_set_size = current_independent_set_size;
    end
    for J = 1:current_independent_set_size
        fprintf(fid, '%d ', maximal_schedule_matrix{I}(J));
    end
    fprintf(fid, '\n');
end
disp('Maximum independent set size is:')
disp(max_independent_set_size);
