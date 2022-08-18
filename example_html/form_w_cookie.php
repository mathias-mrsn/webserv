<?php

session_start();

$name = null;
// permet si logout est activé de supprimer le cookie
// if (!empty($_POST['name']))
//     echo "is set";
// else
//     echo "is not set";
if (!empty($_GET['action']) && $_GET['action'] === 'logout') {
	unset($_COOKIE['User']);
 	setcookie('User', '', time() - 3600); // permet de supprimer le cookie avec une date dans le passe
}
if (!empty($_COOKIE['User']))
{
	$name = htmlspecialchars($_COOKIE['User']);
}
if (!empty($_POST['name']))
{
    $_SESSION['name'] = htmlspecialchars($_POST['name']);
	setcookie("User", $_POST['name'], time()+3600);
	$name = htmlspecialchars($_POST['name']);
}



?>

<?php if ($name) { ?>
	<p>Hello <?php echo($name); ?> !</p>
	<a href="form_w_cookie.php?action=logout">Log out</a>
<?php } else { ?>
	<form action="form_w_cookie.php" method="post">
	<div class "form-group">
		<input class="form-control" name="name" placeholder="Enter your name please.">
	</div>
	<button class="btn btn-primary"> Log in </button>
</form>
<?php } ?>