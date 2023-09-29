function remove_entry(id, path) {
    fetch(path + '/' + String(id), { method: 'DELETE' })
        .then(response => {
            if (response.ok) {
                // Reload the page on successful delete
                location.reload();
            } else {
                // Display an error message
                throw new Error('Failed to delete item');
            }
        }).catch(error => {
            // Display an alert dialog with the error message
            alert(error.message);
        });
}


function update_obj(payload, url) {
    fetch(url, {
        method: "PATCH",
        body: JSON.stringify(payload),
        headers: {
            "Content-Type": "application/json",
        },
    })
        .then((response) => {
            if (response.ok) {
                location.reload();
            } else {
                alert("Failed to create that object.");
            }
        })
        .catch((error) => {
            console.error(error);
            alert("Failed to create that object.");
        });
}


function submit_object(payload, url) {
    fetch(url, {
        method: "POST",
        body: JSON.stringify(payload),
        headers: {
            "Content-Type": "application/json",
        },
    })
        .then((response) => {
            if (response.ok) {
                location.reload();
            } else {
                alert("Failed to create that object.");
            }
        })
        .catch((error) => {
            console.error(error);
            alert("Failed to create that object.");
        });
}

function submit_product() {
    const form = document.getElementById("product-form");

    const formData = new FormData(form);
    const payload = {
        title: formData.get("title"),
        amount: parseInt(formData.get("amount")),
        price: parseFloat(formData.get("price")),
        category_id: parseInt(formData.get("category")),
        description: formData.get("description"),
    };

    submit_object(payload, "/api/product");
    form.reset();
};


function submit_user() {
    const form = document.getElementById("user-form");
    const formData = new FormData(form);
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
    form.reset();
};


function submit_review() {
    const form = document.getElementById("review-form");
    const formData = new FormData(form);
    const payload = {
        user_id: 1, // Not supported yet
        product_id: 21, // Currently not supported
        rating: parseInt(formData.get("rating")),
        content: formData.get("content"),
    };
    submit_object(payload, "/api/review");
    form.reset();
};


function submit_category() {
    const form = document.getElementById("category-form");
    const formData = new FormData(form);
    const payload = {
        name: formData.get("name"),
        is_active: parseInt(formData.get("is_active")) === 1,
    };
    submit_object(payload, "/api/category");
    form.reset();
};
function deleteCookie(name) {
    document.cookie = name + '=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;';
}
