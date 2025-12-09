function textImageData(textRun) {
    const offscreen = new OffscreenCanvas(512, 512);
    const offctx = offscreen.getContext("2d");
    offctx.clearRect(0, 0, 512, 512);
    offctx.beginPath();
    offctx.strokeStyle = "lime";
    offctx.lineWidth = 3;
    offctx.fillStyle = "gold";
    offctx.globalAlpha = 255;
    offctx.font = "44px Georgia";
    offctx.strokeText(textRun, 5, 272);
    offctx.fillText(textRun, 5, 272);
    return offctx.getImageData(0, 0, 512, 512).data;
}