<?php

if (isset($_COOKIE['sessionStart']))
	echo "Session started: ", $_COOKIE['sessionStart'], "<br>";
if (isset($_COOKIE['actualTime']))
	echo "Previous log timestamp: ", $_COOKIE['actualTime'];

?>