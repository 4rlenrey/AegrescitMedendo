async function login() {
    const form = document.getElementById("login-form");
    const formData = new FormData(form);

    const username = formData.get("username");
    const password = formData.get("password");

    let status = await obtainTokens(username, password);
    if (status == true) {
        saveUsername(username); // Save username in localStorage
        saveCookie("access_token", localStorage.getItem("access_token")); // Save access_token in a cookie
        saveCookie("username", username); // Save username in a cookie
        window.location.href = "../";
    }
    form.reset();
}

async function register() {
    const form = document.getElementById("user-form");
    const formData = new FormData(form);

    let username = formData.get("username");
    let password = formData.get("password");

    const payload = {
        name: formData.get("name"),
        username: formData.get("username"),
        password: formData.get("password"),
        surname: formData.get("surname"),
        phone: formData.get("phone"),
        email: formData.get("email"),
        balance: parseFloat(formData.get("balance")),
    };

    submit_object(payload, "/api/user");
    // form.reset();

    let status = await obtainTokens(username, password);
    if (status == true) {
        saveUsername(username); // Save username in localStorage
        saveCookie("access_token", localStorage.getItem("access_token")); // Save access_token in a cookie
        saveCookie("username", username); // Save username in a cookie
        window.location.href = "../";
    }
    // form.reset();
}


function saveUsername(username) {
    localStorage.setItem("username", username);
}

function saveCookie(name, value) {
    document.cookie = `${name}=${value}; path=/`;
}

function getCookie(name) {
    const cookieString = document.cookie;
    const cookies = cookieString.split("; ");
    for (let i = 0; i < cookies.length; i++) {
        const cookie = cookies[i].split("=");
        const cookieName = cookie[0];
        const cookieValue = cookie[1];
        if (cookieName === name) {
            return cookieValue;
        }
    }
    return null;
}

async function handleToken() {
    const accessToken = getCookie("access_token");
    const refreshToken = localStorage.getItem("refresh_token");

    if (!accessToken || isTokenExpired(accessToken)) {
        if (!refreshToken) {
            redirectToLoginPage();
            return;
        }

        const success = await refreshTokens(refreshToken);

        if (!success) {
            redirectToLoginPage();
            return;
        }
    }
}

async function obtainTokens(username, password) {
    const response = await fetch('/auth/obtain', {
        method: 'POST',
        body: JSON.stringify({ username, password }),
        headers: {
            'Content-Type': 'application/json'
        }
    });

    if (response.ok) { 
        const { access_token, refresh_token } = await response.json();
        localStorage.setItem('access_token', access_token);
        localStorage.setItem('refresh_token', refresh_token);
        return true;
    } else {
        return false;
    }
}

async function refreshTokens(path) {
    const refreshToken = localStorage.getItem("refresh_token");
    const username = localStorage.getItem("username");

    const response = await fetch('/auth/refresh', {
        method: 'POST',
        body: JSON.stringify({ refresh_token: refreshToken, username: username }),
        headers: {
            'Content-Type': 'application/json'
        }
    });

    console.log(response)

    if (response.ok) {
        const { access_token } = await response.json();
        localStorage.setItem('access_token', access_token);
        saveCookie("access_token", access_token);
        window.location.replace(window.location.origin + path);
    } else {
        window.location.replace(window.location.origin + path);
    }
}

function isTokenExpired(token) {
    const decodedToken = decodeToken(token);
    const expirationTime = decodedToken && decodedToken.exp * 1000;
    const currentTime = Date.now();
    return expirationTime < currentTime;
}

function decodeToken(token) {
    try {
        const base64Url = token.split('.')[1];
        const base64 = base64Url.replace(/-/g, '+').replace(/_/g, '/');
        const jsonPayload = decodeURIComponent(
            atob(base64)
                .split('')
                .map((c) => '%' + ('00' + c.charCodeAt(0).toString(16)).slice(-2))
                .join('')
        );
        return JSON.parse(jsonPayload);
    } catch (error) {
        return null;
    }
}

function redirectToLoginPage() {
    window.location.href = '/login';
}
