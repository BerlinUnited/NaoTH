from naoth.log import Reader as LogReader


def write(f, name, out):
    position, size, _ = f.messages[name]
    f.reader.log.seek(position)
    data = f.reader.readBytes(size)

    out.write(f.number.to_bytes(4, byteorder='little', signed=False))
    out.write(name.encode('ascii'))
    out.write(b'\0')
    out.write(size.to_bytes(4, byteorder='little', signed=False))
    out.write(data)


if __name__ == "__main__":
    logFilePath = "game.log"

    out = open("audio_data.log", "wb")

    for f in LogReader(logFilePath):
        if "AudioData" in f.messages or "WhistlePercept" in f.messages:
            write(f, "FrameInfo", out)
        if "AudioData" in f.messages:
            write(f, "AudioData", out)
        if "WhistlePercept" in f.messages:
            write(f, "WhistlePercept", out)

    out.close()
