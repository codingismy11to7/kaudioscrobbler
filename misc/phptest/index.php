<?

header( "Content-type: text/plain" );

function hs_uptodate(){
?>
UPTODATE
57575757575757575757575757575757
http://progoth/~progoth/kas/v1.1.php
INTERVAL 6
<?
}

function hs_update(){
?>
UPDATE http://localhost
57575757575757575757575757575757
http://progoth/~progoth/kas/v1.1.php
INTERVAL 6
<?
}

function hs_baduser(){
?>
BADUSER
INTERVAL 6
<?
}

function hs_failed(){
?>
FAILED not written yet yo
INTERVAL 3
<?
}

function unparse(){
?>
uteaonhutoansuhoaeu
eau
aou
oa
u
<?
}

if( strtolower( $_GET["u"] ) == "progothdevtest" )
    hs_uptodate();
else
    hs_baduser();

?>
