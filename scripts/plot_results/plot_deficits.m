function plot_deficits(show_conf,filename)

% PLOT_DEFICITS Plots deficits for all links
%     To be used with deficit files
%
%     PLOT_STABILITY(show_conf,filename)
% 
%     show_conf: =0 if network configuration is not to be displayed
%     filename: file to be plotted

[pathstr,name,ext] = fileparts(filename);   % Remove extension
filename_fig = sprintf('./%s.fig', name);
if exist([pwd filesep filename_fig], 'file')            % Check current folder only
    open(filename_fig);
else
    str_size = 'Network size: ';
    str_deficits = '==================Deficits===================';
    fid = fopen(filename);

    tline = fgetl(fid);
    while ~strcmp(tline,str_deficits)
        k = strfind(tline, str_size);
        if ~isempty(k)
            net_size = sscanf(tline, [str_size '%d']);
        end
        if (show_conf)
            disp(tline)
        end
        tline = fgetl(fid);
    end
    deficits = fscanf(fid,'%d', [net_size,Inf] );

    fclose(fid);

    % Dock figures by default
    set(0,'DefaultFigureWindowStyle','docked');
    % Set default values for the plots
    sim_time = size(deficits,2);
    location = 'NorthEast';
    orientation = 'vertical';
    font_size = 26;
    paper_position = [0.25 0.25 10.5 8];
    axes_position = [0.15 0.12 0.8 0.8];
    fontname = 'Times New Roman';
    xscale = 'linear';
    yscale = 'linear';
    set(0,'DefaultAxesFontName',fontname);
    set(0,'DefaultTextFontName',fontname);
    set(0,'DefaultFigureVisible','on');
    set(0,'DefaultAxesColorOrder', [0 0 1; 0 0.5 0; 1 0 0; 0 0.75 0.75; 0.75 0 0.75; 0.75 0.75 0; 0 0.5 1; 1 0.5 0; 0.75 0 0; 0 1 0.5]);

    % Plot deficits
    figure;
    hold all;
    T_sample = (sim_time-rem(sim_time,5))/5;
    local_xvalues = 0:T_sample:sim_time;
    local_xticks = local_xvalues;
    hp = plot(1:sim_time, deficits, '-', 'LineWidth', 2);

    set(gcf, 'PaperOrientation', 'landscape', 'PaperPosition', paper_position);
    set(gca, 'FontSize', font_size, 'XLim', [0 sim_time], 'XTick', local_xticks,...
     'XScale', xscale, 'YScale', yscale, 'XGrid', 'on', 'YGrid', 'on',...
     ...'YLim', [0 0.35], 'YTick', [0 0.05 0.1 0.15 0.2 0.25 0.3 0.35],...
     'YMinorTick', 'off', 'YMinorGrid', 'off', 'Box', 'on');
    disp_legends = cell(net_size,1);
    for i=1:net_size
        disp_legends{i} = ['L' num2str(i)];
    end
    hl=legend(hp, disp_legends, 'Location', location, 'Orientation', orientation);
    set(hl, 'Interpreter', 'LaTex', 'FontSize', font_size);
    local_label = ['Time (slots)'];
    xlabel(local_label, 'Interpreter', 'LaTex', 'FontSize', font_size);
    ylabel('Deficits', 'Interpreter', 'LaTex', 'FontSize', font_size);
    set(gca, 'Units', 'Normalized', 'Position', axes_position);
    [pathstr,filename,ext] = fileparts(filename);   % Remove extension
    title(filename);
    filename_fig = sprintf('./%s.fig', filename);
    filename_eps = sprintf('./%s.eps', filename);
    filename_pdf = sprintf('./%s.pdf', filename);
    saveas(gcf, filename_fig);
    saveas(gcf, filename_eps, 'epsc'); %Black and white: 'eps' Color: 'epsc'
    if ispc
       system(['ps2pdf -dPDFSETTINGS#/prepress ' filename_eps ' ' filename_pdf]); % If running on a Windows machine
    else
       system(['ps2pdf -dPDFSETTINGS=/prepress ' filename_eps ' ' filename_pdf]); % If running on a Linux/Mac machine
    end
    system(['pdftops -eps ' filename_pdf ' ' filename_eps]);
end
