<?php

/*  
--Copyright (C) 2007 <SWGEmu>
 
--This File is intended to be used with Core3 only.
 
--This program is free software; you can redistribute 
--it and/or modify it under the terms of the GNU Lesser 
--General Public License as published by the Free Software
--Foundation; either version 2 of the License, 
--or (at your option) any later version.
 
--This program is distributed in the hope that it will be useful, 
--but WITHOUT ANY WARRANTY; without even the implied warranty of 
--MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
--See the GNU Lesser General Public License for
--more details.

Requires atleast revision 613 of Core3

Enjoy. Maximilius (CSR).
*/

#----------------------------------------- CUSTOM USER CONFIGURATION -----------------------------------------------#

$serverTitle = "SWGemu Test Centre"; #Edit this field with your server's name. It can be anything you wish.#
$serverAddress = "chicago2.swgemu.com"; #Edit this field with your Core3 server's IP address or hostname.#

#------------------------------------------ DO NOT EDIT BELOW THIS LINE -------------------------------------------#

$port = 44455;

require("simplexml.class.php");

$fp = @fsockopen("tcp://" . $serverAddress, $port, $errno, $errstr);

if (!$fp) {
    $status = "down";
} else {
    fwrite($fp, "\n");
    $xml = fread($fp, 99999);
    fclose($fp);
    
    $sxml = new simplexml;
    $xmldata = $sxml->xml_load_data($xml);
    $status = $xmldata->status;
}

if ($status == "down") {

    $now = date("D, d M Y H:i:s T");

$output = "<?xml version=\"1.0\"?>
            <rss version=\"2.0\">
                <channel>
                    <title>$serverTitle Status</title>
                    <link>http://www.swgemu.com/</link>
                    <description>Live, Up to date server status for $serverTitle.</description>
                    <language>en-us</language>
                    <pubDate>$now</pubDate>
                    <lastBuildDate>$now</lastBuildDate>
                    <docs>http://support.swgemu.com</docs>
                    <managingEditor>Maximilius-CSR@gmail.com</managingEditor>
                    <webMaster>Maximilius-CSR@gmail.com</webMaster>
            ";
            
    $output .= "<item><title>$serverTitle: OFFLINE</title>
                    <link>http://support.swgemu.com</link>
                    
<description>The automatic server status reads as OFFLINE.</description>
                </item>";

$output .= "</channel></rss>";
header("Content-Type: application/rss+xml");
echo $output;

    exit(1);
}

$now = date("D, d M Y H:i:s T");

$output = "<?xml version=\"1.0\"?>
            <rss version=\"2.0\">
                <channel>
                    <title>$serverTitle Status</title>
                    <link>http://www.swgemu.com/</link>
                    <description>Live, Up to date Status report for $serverTitle</description>
                    <language>en-us</language>
                    <pubDate>$now</pubDate>
                    <lastBuildDate>$now</lastBuildDate>
                    <docs>http://support.swgemu.com</docs>
                    <managingEditor>Maximilius-CSR@gmail.com</managingEditor>
                    <webMaster>Maximilius-CSR@gmail.com</webMaster>
            ";
            
    $output .= "<item><title>$serverTitle: ONLINE</title>
                    <link>http://support.swgemu.com</link>
                    
<description>
The server is ".$status.". 
There are " . $xmldata->users->connected . " Users Connected.
This was gathered on " . date("F j, Y, g:i:s a", $xmldata->timestamp).".
</description>
                </item>";

$output .= "</channel></rss>";
header("Content-Type: application/rss+xml");
echo $output;

?>
