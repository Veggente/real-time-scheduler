function plot_stability(filename)

% PLOT_STABILITY Plots stability results
%     To be used with files './stability_output_prefix*'
%
%     PLOT_STABILITY(filename)
% 
%     filename: file to be plotted

[pathstr,name,ext] = fileparts(filename);   % Remove extension
filename_fig = sprintf('./%s.fig', name);
if exist([pwd filesep filename_fig], 'file')            % Check current folder only
    open(filename_fig);
else
    stability = load(filename);

    % Dock figures by default
    set(0,'DefaultFigureWindowStyle','docked');
    % Set default values for the plots
    simulations = size(stability,1);
    location = 'NorthEast';
    orientation = 'vertical';
    font_size = 26;
    paper_position = [0.25 0.25 10.5 8];
    axes_position = [0.15 0.12 0.8 0.8];
    fontname = 'Times New Roman';
    xscale = 'linear';
    yscale = 'log';
    set(0,'DefaultAxesFontName',fontname);
    set(0,'DefaultTextFontName',fontname);
    set(0,'DefaultFigureVisible','on');
    set(0,'DefaultAxesColorOrder', [0 0 1; 0 0.5 0; 1 0 0; 0 0.75 0.75; 0.75 0 0.75; 0.75 0.75 0; 0 0.5 1; 1 0.5 0; 0.75 0 0; 0 1 0.5]);

    % Plot stability
    figure;
    hold all;
    sample = (simulations-rem(simulations,5))/5;
    local_xvalues = stability(:,2);
    local_xticks = local_xvalues;
    y_up = numel(num2str( max( stability(:,4) ) ))+1;
    y_down = -1;
    hp = plot(local_xvalues, stability(:,3:5), '-', 'LineWidth', 2);

    set(gcf, 'PaperOrientation', 'landscape', 'PaperPosition', paper_position);
    set(gca, 'FontSize', font_size, 'XLim', [min(local_xvalues) max(local_xvalues)], 'XTick', local_xticks,...
     'XScale', xscale, 'YScale', yscale, 'XGrid', 'on', 'YGrid', 'on',...
     'YLim', [10^y_down 10^y_up],... 'YTick', [0 0.05 0.1 0.15 0.2 0.25 0.3 0.35],...
     'YMinorTick', 'off', 'YMinorGrid', 'on', 'Box', 'on');
    disp_legends = cell(3,1);
    disp_legends{1} = 'Ratio Stability';
    disp_legends{2} = 'Cumulative arrivals';
    disp_legends{3} = 'Cumulative departures';
    hl=legend(hp, disp_legends, 'Location', location, 'Orientation', orientation);
    set(hl, 'Interpreter', 'LaTex', 'FontSize', font_size);
    local_label = ['Ratio QoS'];
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
