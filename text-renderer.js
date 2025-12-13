function textImageData(textRun) {
    const offctx = offscreen.getContext("2d", { willReadFrequently: true });
    offctx.strokeStyle = "lime";
    offctx.lineWidth = 3;
    offctx.fillStyle = "gold";
    offctx.globalAlpha = 255;
    offctx.font = "44px Georgia";
    offctx.strokeText(textRun, 5, 272);
    offctx.fillText(textRun, 5, 272);
    return offctx.getImageData(0, 0, 512, 512).data;
}