<?

header( "Content-type: text/plain" );


if( !$_POST )
    failed();
else
{
    $f = fopen( "/tmp/lastpost.txt", "w" );
    fwrite( $f, $_POST );
    fclose( $f );
    handle_post();
}

function failed(){
?>
FAILED hey your client sux yo
INTERVAL 6
<?
}

function handle_post()
{
    $md5 = md5( md5( "password" ) . "57575757575757575757575757575757" );
    if( strtolower( $_POST["u"] ) == "progothdevtest" && $_POST["s"] == $md5 )
    {
?>
OK
INTERVAL 3
<?
    }
    else
    {
?>
BADAUTH
INTERVAL 6
<?
    } 
}
?>