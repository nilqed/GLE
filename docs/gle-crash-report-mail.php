<?php
if (preg_match('/^5568$/', $_POST["code"])) {
	$to = "jstruyf32@struyf-ye.org";
	$subject = "GLE Crash Report";   
	if (mail($to, $subject, $_POST["report"])) {
		echo("Your crash report has been successfully sent.\n");
	} else {
		echo("An error occured while sending your crash report.\n");
		echo("Please contact the GLE mailing list to inform us about this error.\n");
	}
}

if (!preg_match('/4.2.3/', $_POST["version"])) {
	echo("\nA more recent version of GLE is available here:\n");
	echo("\nhttp://www.gle-graphics.org/\n");
	echo("\nPlease consider upgrading; this problem may have been resolved in the mean time.\n");
}
?>

Thank you for taking the time to prepare this report.

GLE version: <?php echo $_POST["version"] ?>.
