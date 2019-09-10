function fig2gle(FileName,FigureHandle,varargin)
%% fig2gle.m
%
% This file is used to strip the data from a MATLAB/Octave figure and
% generate and equivalent GLE plot.  This file is capable of handling
% figures with subplots. It can identify plots, legends, labels,
% and titles.
%
% fig2gle(FileName,FigureHanle,varagin)
%
% FileName     is a string with the file name without directory
%              that will be used for the GLE and PDF files.  Note,
%              to work properly, you should already be in the
%              directory where you wish to save the files.  The
%              script generates a directory named FileName in the
%              current directory to save all the CSV data files,
%              the GLE file, and the PDF file (if requested).
%
% FigureHandle is the handle of the figure you wish to generate a
%              GLE file for.
%
% vargin       includes the PDF_Flag which if set and equal to 1
%              will generate a PDF image if GLE is installed.
%
% Standard Usage:
%
%  1) plot a figure in MATLAB or Octave
%  2) make it the figure active or simply pass the handle to the
%     command
%  3) execute the command: fig2gle('GLE_File',gcf)
%  4) The local directory will have a subdirectory named GLE_File
%     and within that subdirectory will appear the following:
%          subplot#p*.dat - CSV data file(s) for each plot where #
%                           is the subplot number and * is the
%                           number of one of plots within that
%                           subplot (e.g. subplot1p1.csv
%                           corresponds to the data for the first
%                           plot withint the first subplot)
%          GLE_File.gle   - the GLE script
%          GLE_File.pdf   - PDF figure
%
% Written in January 2009 by Javier A. Kypuros, Ph.D.
%
% This script has been tested using both MATLAB and Octave with 2D
% plots. If you find any errors or have any suggestions you can
% contact me at jakypuros@utpa.edu.

%% Initialize GLE Folder
% A folder is created to hold all the pertinent files for
% generating the plot in GLE including data files, the GLE script,
% and a PDF copy of the resulting figure.
if ~exist(FileName,'dir')
    mkdir(FileName);
else
    rmdir(FileName,'s');
    mkdir(FileName);
end

% Determine if you are working in MATLAB or Octave.
OCTAVE_FLAG = exist('OCTAVE_VERSION'); 

%% Gather Plot Handles
% Plot handles appear in reverse order (from right to left,
% bottom to top). Thus the order must be corrected.
figure(FigureHandle);
if (OCTAVE_FLAG ~= 0)
    f = get(FigureHandle,'Children');
    Ngraphs = length(f);
    % Octave plot handles seem to be always negative
    f = sort(f,'descend');
else
    % Legend handles
    L = findobj(FigureHandle,'Type','axes','Tag','legend');
    L = sort(L);
    % Axes handles
    f = findobj(FigureHandle,'Type','axes','Tag','');
    Ngraphs = length(f);
    f = sort(f);
end

%% Request Subplot Layout From User
if Ngraphs >= 1
    disp(sprintf(['NOTE %d subplots have been detected!! Please ' ...
                  'tell me how they are arranged in rows and ' ...
                   'columns...'],Ngraphs))
    rows = input('How many rows of subplots are there? ','s');
    cols = input('How many columns of subplots are there? ','s');
    rows = str2num(rows); cols = str2num(cols);
else
    rows = 1; cols = 1;
end

%% Determine Legend Keys in MATLAB
% Unlike Octave, MATLAB treats the legends as axes and thus
% in the figure structure they appear independent of the subplot
% axes and are not directly linked. They are positioned to appear
% as independent axes above the corresponding subplot. Therefore,
% if you are using MATLAB we must determine which subplot a lengend
% corresponds to.  This is done by checking the position of the
% legend object.  The 'position' handle gives the relative position
% of the legend axes using a unit scale.  In other words, the width
% and length of the figure are 1.  If a legend appears in the first
% subplot of a 2 by 2 arrangement, then the x-position of the
% legend shoudl be less than 0.5 and the y-position more than 0.5
% where x and y are measured from the lower left corner.
if (OCTAVE_FLAG == 0)
    % Generate a matrix to determine subplot order
    GraphCount = 1;
    % Note that the subplots are numbered from left to right and
    % top to bottom.  However, the position, which is being used
    % below to determine the placement of the legend is measured
    % from the bottom left corner, hence the reason the rows are
    % reversed to generate the matrix A.
    for p = rows:-1:1
        for q = 1:cols
            A(p,q) = GraphCount;
            GraphCount = GraphCount+1;
        end
    end
    % Initialize cell array that will house the MATLAB legend keys
    for ii = 1:length(f)
        PlotLegends{ii} = [];
    end
    for jj = 1:length(L)
        LgndPos = get(L(jj),'Position');
        LgndRow = ceil(LgndPos(2)*rows);
        LgndCol = ceil(LgndPos(1)*cols);
        %[ii LgndPos(1:2) LgndRow LgndCol]
        PlotLegends{A(LgndRow,LgndCol)} = get(L(jj),'String');
    end	
end

%% Initialize GLE File
% I tend to use LaTeX with the Times font package. Thus, the lines
% below create a tex preamblel to use the 'pslatex' and 'mathptmx'
% packages for typsetting the text and mathematical symbols.
GLE_File = sprintf('%s/%s.gle',FileName,FileName); 
fid = fopen(GLE_File,'wt');
paper_position = get(FigureHandle,'PaperPosition');
graph_size = paper_position(3:4)*2.54;
fprintf(fid,'!GLE file to plot MATLAB figure in %s.gle\n',FileName);
fprintf(fid,'!Created on %s\n\n',date);
fprintf(fid,'size %4.2f %4.2f\n',graph_size);
plot_title_hei = graph_size(2)/30;
x_label_hei = graph_size(2)/30;
y_label_hei = graph_size(2)/30;
fprintf(fid,'begin texpreamble\n');
fprintf(fid,['\t\\usepackage{pslatex,mathptmx}\n']);
fprintf(fid,'end texpreamble\n');
fprintf(fid,'set texlabels 1\n\n');

%% Determine Subplot Increments
% This section determine the size of the divisions for each subplot
% based on the overall figure size and number of rows and columns.
DeltaX = graph_size(1)/cols;
DeltaY = graph_size(2)/rows;
XPos = 0;
YPos = graph_size(2)-DeltaY;

%% Generate Subplots
% Here the script cycles through each subplot to gather the
% pertinent data and identities to generate the GLE scrip for each
% graph. It cycle through the handles for each subplot and
% generates a GLE graph section for each. The script is capable of
% identifies line color, line width, markers, axis labels, axis
% scales (log or linear), and keys.
g = get(f,'Children');
RowIndex = 1;
ColIndex = 1;
for j = 1:Ngraphs
    Nplots = length(g{j});
    % gather subplot-specific data
    x_data = get(g{j},'XData');
    y_data = get(g{j},'YData');
    x_scale = get(f(j),'XScale');
    y_scale = get(f(j),'YScale');
    x_grid = get(f(j),'XGrid');
    y_grid = get(f(j),'YGrid');
    plot_title =  get(get(f(j),'Title'),'String');
    x_label = get(get(f(j),'XLabel'),'String');
    y_label = get(get(f(j),'YLabel'),'String');
    x_lim = get(f(j),'XLim');
    y_lim = get(f(j),'YLim');
    % gather line-specific data
    line_style = get(g{j},'LineStyle');
    line_color = get(g{j},'Color');
    line_width = get(g{j},'LineWidth');
    marker = get(g{j},'marker');
    % gather legend strings
    if OCTAVE_FLAG > 0
	keys = get(get(f(j),'Children'),'keylabel');
    else
	keys = PlotLegends{j};
    end	
    % initialize subplot graph
    fprintf(fid,'amove %4.2f %4.2f\n',XPos,YPos);
    fprintf(fid,'begin graph\n');
    fprintf(fid,'\tsize %3.1f %3.1f\n',DeltaX,DeltaY);
    fprintf(fid,'\thscale auto\n');
    fprintf(fid,'\tvscale auto\n');
    % load CSV data files
    if Nplots == 1
        M = [];
        CSV_FileName = ['subplot' num2str(j) 'p1.csv'];
	DatFileName = [FileName '/' CSV_FileName];
	M = [x_data' y_data'];
	dlmwrite(DatFileName,M,'precision','%10.5f');
	fprintf(fid,'\tdata "%s"\n',CSV_FileName);
    else
        for k = 1:Nplots
            M = [];
            CSV_FileName = ['subplot' num2str(j) 'p' num2str(k) '.csv'];
            DatFileName = [FileName '/' CSV_FileName];
            M = [x_data{k}' y_data{k}'];
            dlmwrite(DatFileName,M,'precision','%10.5f');
            fprintf(fid,'\tdata "%s"\n',CSV_FileName);
        end
    end
    % place title and axis labels
    if ~isempty(plot_title)
	fprintf(fid,'\ttitle "%s" hei %5.3f\n',plot_title,plot_title_hei);
    end
    if ~isempty(x_label)
	fprintf(fid,'\txtitle "%s" hei %5.3f\n',x_label,x_label_hei);
    end
    if ~isempty(y_label)
	fprintf(fid,'\tytitle "%s" hei %5.3f\n',y_label,y_label_hei);
    end
    % specify x-axis limits, scale, and grid
    gle_xaxis = sprintf('\txaxis min %d max %d hei %2.3f\n',x_lim, ...
                        x_label_hei);
    if strcmp(x_scale,'log')
	gle_xaxis = ...
            sprintf(strcat(gle_xaxis(1:length(gle_xaxis)-1),' log\n'));
    end
    if strcmp(x_grid,'on')
        fprintf(fid,'\txticks lstyle 2\n');
        fprintf(fid,'\txsubticks lstyle 2\n');
        gle_xaxis = ...
            sprintf(strcat(gle_xaxis(1:length(gle_xaxis)-1),' grid\n'));
    end
    fprintf(fid,gle_xaxis);
    % specify y-axis limits, scale, and grid
    gle_yaxis = sprintf('\tyaxis min %d max %d hei %2.3f\n',y_lim, ...
                        y_label_hei);
    if strcmp(y_scale,'log')
	gle_yaxis = ...
            sprinft(strcat(gle_yaxis(1:length(gle_yaxis)-1),' log\n'));
    end
    if strcmp(y_grid,'on')
        fprintf(fid,'\tyticks lstyle 2\n');
        fprintf(fid,'\tysubticks lstyle 2\n');
        gle_yaxis = ...
            sprintf(strcat(gle_yaxis(1:length(gle_yaxis)-1),' grid\n'));
    end
    fprintf(fid,gle_yaxis);
    % generate plot command(s) (i.e. generate the d# commands)
    if Nplots == 1
	fprintf(fid,gle_plot_cmd(1,line_color,line_style, ...
                                 line_width,marker,keys));
    else
        for l = 1:Nplots
            LineColor = line_color{l};
            LineStyle = line_style{l};
            LineWidth = line_width{l};
            Marker = marker{l};
            Key = keys{l};
            fprintf(fid,gle_plot_cmd(l,LineColor,LineStyle, ...
                                    LineWidth,Marker,Key));
        end 
    end
    % terminate current subplot
    fprintf(fid,'end graph\n\n');
    % index to next subplot
    if (ColIndex < cols)
        ColIndex = ColIndex+1;
        XPos = XPos + DeltaX;
    else
        RowIndex = RowIndex+1;
        ColIndex = 1;
        XPos = 0;
        YPos = YPos - DeltaY;
    end
end
%% Close GLE File
fclose(fid);
%% Generate PDF
PDF_Flag = varargin{1};
if ~isempty(PDF_Flag) && (PDF_Flag == 1)
    [status, output] = system(sprintf('gle -d pdf %s',GLE_File));
    if status == 0
        disp(sprintf(['PDF GENERATED!! \nYour PDF file was saved to ' ...
                      '%s/%s.'],pwd,GLE_File));
    else
        disp(sprintf(['Failed to generate PDF. \nThe GLE output ' ...
                      'follows.\n']));
        output
    end
end

function d_line = gle_plot_cmd(l,line_color,line_style,line_width,marker,key)
%% gle_plot_cmd
% This function is used to covert MATLAB/Octave parameters into a GLE
% plot command.  It returns a "d" command line for each plot.

% This section is used to determine the line color.
    gle_line_color = sprintf('rgb255(%d,%d,%d)',round(line_color* ...
                                                      255));
% This section is used to determine the line width.
    gle_line_width = line_width/72*2.54;
% This section is used to determine the line style.
switch line_style
 case ':'                                  
  gle_line_style = 6;
 case '-'
  gle_line_style = 1;
 case '-.'
  gle_line_style = 8;
 case '--'
  gle_line_style = 3;
 case 'none'
  gle_line_style = [];
 otherwise
  gle_line_style = 1;
end
% This section is used to determine the marker.
switch marker
 case '+'
  gle_marker = 'plus';
 case '*'
  gle_marker = 'asterisk';
 case 'o'
  gle_marker = 'circle';
 case '*'
  gle_marker = 'asterisk';
 case 'x'
  gle_marker = 'cross';
 case '^'
  gle_marker = 'triangle';
 case '.'
  gle_marker = 'dot';
 otherwise
  gle_marker = [];
end
% Generate "d#" line command
if isempty(gle_line_style) % marker with no line
    d_line = sprintf('\td%d marker %s color %s\n',l,gle_marker, ...
                     gle_line_color);
elseif isempty(gle_marker) % line with no marker
    d_line = sprintf(['\td%d line lstyle %d color %s lwidth ' ...
                      '%1.2f\n'],l,gle_line_style,gle_line_color, ...
                     gle_line_width);
else % line with marker
    d_line = sprintf(['\td%d line lstyle %d color %s lwidth ' ...
                      '%1.2f marker %s\n'],l,gle_line_style, ...
                     gle_line_color,gle_line_width,gle_marker);
end
% Add key if present
if ~isempty(key)
    key_string = sprintf(' key "%s"\\n',key);
    % The \\ is used to insure that "\n" is carried over to the
    % following sprintf statement
    d_line = sprintf(strcat(d_line(1:(length(d_line)-1)), ...
                            key_string));
end
end

end

    
    
    
    
