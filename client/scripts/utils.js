function insertElements(colors) {
  if ("content" in document.createElement("template")) {
    const container = document.querySelector("#led-container");
    var template = document.querySelector("#template-led");

    colors.forEach((color) => {
      // Clone the container from template
      const clone = template.content.cloneNode(true);
      const h5 = clone.querySelector("h5");
      const img = clone.querySelector("img");
      const a = clone.querySelector("a");

      h5.textContent = color.charAt(0).toUpperCase() + color.slice(1);
      h5.classList.add(`${color}-text`);

      img.id = `led-${color}`;
      img.addEventListener("click", function (e) {
        toggleLed(color);
      });

      a.id = `button-${color}`;
      a.addEventListener("click", function (e) {
        toggleLed(color);
      });

      // Append the new row into container
      container.appendChild(clone);
    });
  } else {
    alert("Your browser does not support template element!");
  }
}

function toggleLed(color) {
  statusObj[color] = statusObj[color] === 0 ? 1 : 0;
  client.publish("esp32/gooner", JSON.stringify(statusObj));

  setLedAndButton(color);
}

function setLedAndButton(color) {
  const status = statusObj[color];
  const button = buttons[`button-${color}`];
  const ledImage = ledImageArray[`led-${color}`];

  if (!!status) {
    button.textContent = "OFF";
    button.classList.add("red");
    ledImage.src = `/images/${color}.png`;
  } else {
    button.textContent = "ON";
    button.classList.remove("red");
    ledImage.src = `/images/OFF.png`;
  }
}

function getElements(selector) {
  return [...document.querySelectorAll(selector)].reduce(
    (acc, curr) => ((acc[curr.id] = curr), acc),
    {}
  );
}
