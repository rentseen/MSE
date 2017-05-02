<%@ page language="java" contentType="text/html; charset=UTF-8"
    pageEncoding="UTF-8"%>
<!DOCTYPE HTML>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>Register</title>
<script src="${pageContext.request.contextPath}/js/jquery.min.js"
	type="text/javascript"></script>
<script src="${pageContext.request.contextPath}/js/jquery-ui.js"
	type="text/javascript"></script>
<link rel="stylesheet"
	href="${pageContext.request.contextPath}/css/bootstrap.min.css">
<link rel="stylesheet"
	href="${pageContext.request.contextPath}/css/jquery-ui.css">
</head>
<body>
	<div class="container">
		<form action="register.action" method="post">
		<br>
		<br>
			<div class="row">
				<div class="col-md-4"></div>
				<div class="col-md-3">
					<input type="text" name="username" class="form-control" placeholder="account" /> 
					<br>
					<input type="password" name="password" class="form-control" placeholder="password" />
					<br>
				</div>
			</div>
			<div class="row">
				<div class="col-md-6">
				</div>
				<div class="col-md-1">
					<button class="btn btn-default btn-block" type="submit">注册</button>
				</div>
			</div>
		</form>
		<br>
		<br>

		<!-- <a href="gotoHome.action">首页</a> -->
	</div>
</body>
</html>