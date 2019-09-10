<?php
include_once "../glepoll/poll_cookie.php";
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<HTML>
<HEAD>
<TITLE> Welcome </TITLE>
<META NAME="Generator" CONTENT="EditPlus">
<META NAME="Author" CONTENT="">
<META NAME="Keywords" CONTENT="">
<META NAME="Description" CONTENT="">
</HEAD>


<BODY>
<link href="gle.css" rel="stylesheet" type="text/css">
<font face="Verdana, Arial, Lucida Sans, Helvetica" size="-1">
<center>
&nbsp;<br>
<img src="../images/logo.png">
<P>
<h3>Graphics Layout Engine</h3>
<p>
</center>
<p>

<h3>About</h3>

GLE (Graphics Layout Engine) is a graphics scripting language designed for creating publication quality graphs, plots, diagrams, figures and slides. GLE supports various graph types (function plots, histograms, bar graphs, scatter plots, contour lines, color maps, surface plots, ...) through a simple but flexible set of graphing commands. More complex output can be created by relying on GLE's scripting language, which is full featured with subroutines, variables, and logic control. GLE relies on LaTeX for text output and supports mathematical formulea in graphs and figures. GLE's output formats include EPS, PS, PDF, JPEG, and PNG. GLE is licenced under the <a href="license.html">BSD license</a>. QGLE, the GLE user interface, is licenced under the <a href="license.html">GPL license</a>.

<h3>New in GLE 4.1.0</h3>

<table>
<tr>
<td><img src="../screenshots/qgle0.2-asbudden-150x150.png" width="150" height="150"></td>
<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>

<td><img src="../screenshots/bar3d-scale-150x150.png" width="150" height="150"></td>
<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
<td><img src="../screenshots/curvedarrow-150x150.png" width="150" height="150"></td>
<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
<td><img src="../screenshots/dt-150x150.png" width="150" height="150"></td>
<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
<td>
<div align=center>
<?php
$poll_path = dirname(__FILE__);

require_once $poll_path."/../glepoll/include/config.inc.php";
require_once $poll_path."/../glepoll/include/$POLLDB[class]";
require_once $poll_path."/../glepoll/include/class_poll.php";
$CLASS["db"] = new polldb_sql;
$CLASS["db"]->connect();

$php_poll = new poll();

$php_poll->set_template_set("simple");
$php_poll->set_max_bar_length(80);
echo $php_poll->poll_process(0);

?>
</div>
</td>

</tr>
<tr>
<td><a href="../screenshots/qgle0.2-asbudden.png">QGLE edits GLE scripts</a></td>
<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
<td><a href="../examples/2dplots/bar3d.html">Auto scale + Auto key/labels</a></td>
<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
<td><a href="../examples/other/arrowstyle.html">Curved arrow heads</a></td>
<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
<td><a href="../examples/diagrams/playtennis_dt.html">Begin/end object blocks</a></td>
<td>&nbsp;</td>
<td>"Easy import of GLE figures in MS Word."</td>
</tr>
</table>

</font>
</BODY>
</HTML>
