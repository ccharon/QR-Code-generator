/* 
 * QR Code generator demo (C++)
 * 
 * Run this command-line program with no arguments. The program computes a bunch of demonstration
 * QR Codes and prints them to the console. Also, the SVG code for one QR Code is printed as a sample.
 * 
 * Copyright (c) Project Nayuki. (MIT License)
 * https://www.nayuki.io/page/qr-code-generator-library
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise, arising from,
 *   out of or in connection with the Software or the use or other dealings in the
 *   Software.
 * 
 * 2025-04-07, Charon: modified sources to compile with watcom 1.9 C++ compiler
 */

#include <climits>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <dos.h>
#include <conio.h>
#include "qrcodegen.hpp"

using qrcodegen::QrCode;
using qrcodegen::QrSegment;


// Function prototypes
static void doBasicGraphicsDemo(const char *text);
static void doBasicDemo();
static void doVarietyDemo();
static void doSegmentDemo();
static void doMaskDemo();
static std::string toSvgString(const QrCode &qr, int border);
static void printQr(const QrCode &qr);
static void printQrCGA(const QrCode &qr, const char *info);
static void setPixel(int x, int y, int color);
static void setVideoMode(unsigned char mode);
static uint8_t getVideoMode();

// The main application program.
int main(int argc, char *argv[]) {
	const char *text = "https://www.nayuki.io/";
	
	if (argc == 2)
        text = argv[1];
	
	doBasicGraphicsDemo(text);
	doBasicDemo();
	doVarietyDemo();	
	doSegmentDemo();
	doMaskDemo();
	
	return EXIT_SUCCESS;
}


/*---- Demo suite ----*/
static void doBasicGraphicsDemo(const char *text) {
	const QrCode::Ecc errCorLvl = QrCode::LOW;       // Error correction level

	// Make and print the QR Code symbol
	const QrCode qr = QrCode::encodeText(text, errCorLvl);
	printQrCGA(qr, text);
}


// Creates a single QR Code, then prints it to the console.
static void doBasicDemo() {
	const char *text = "Hello, world!";              // User-supplied text
	const QrCode::Ecc errCorLvl = QrCode::LOW;       // Error correction level
	
	// Make and print the QR Code symbol
	const QrCode qr = QrCode::encodeText(text, errCorLvl);
	printQrCGA(qr, text);

	std::cout << toSvgString(qr, 4).c_str() << std::endl;

	// UTF-8 binary-encoded version of "https://www.heise.de"
	std::vector<unsigned char> urlBytes;
	urlBytes.push_back(0x68);
	urlBytes.push_back(0x74);
	urlBytes.push_back(0x74);
	urlBytes.push_back(0x70);
	urlBytes.push_back(0x73);
	urlBytes.push_back(0x3A);
	urlBytes.push_back(0x2F);
	urlBytes.push_back(0x2F);
	urlBytes.push_back(0x77);
	urlBytes.push_back(0x77);
	urlBytes.push_back(0x77);
	urlBytes.push_back(0x2E);
	urlBytes.push_back(0x68);
	urlBytes.push_back(0x65);
	urlBytes.push_back(0x69);
	urlBytes.push_back(0x73);
	urlBytes.push_back(0x65);
	urlBytes.push_back(0x2E);
	urlBytes.push_back(0x64);
	urlBytes.push_back(0x65);
	
	// Generate the QR code using the binary data
	const QrCode qr2 = QrCode::encodeBinary(urlBytes, errCorLvl);
	printQrCGA(qr2, "UTF-8 binary-encoded version of https://www.heise.de");
}

// Creates a variety of QR Codes that exercise different features of the library, and prints each one to the console.
static void doVarietyDemo() {
	// Numeric mode encoding (3.33 bits per digit)
	const QrCode qr0 = QrCode::encodeText("314159265358979323846264338327950288419716939937510", QrCode::MEDIUM);
	printQrCGA(qr0, "Numeric mode encoding (3.33 bits per digit)");
	
	// Alphanumeric mode encoding (5.5 bits per character)
	const QrCode qr1 = QrCode::encodeText("DOLLAR-AMOUNT:$39.87 PERCENTAGE:100.00% OPERATIONS:+-*/", QrCode::HIGH);
	printQrCGA(qr1, "Alphanumeric mode encoding (5.5 bits per character)");
	
	// Unicode text as UTF-8
	const QrCode qr2 = QrCode::encodeText("\xE3\x81\x93\xE3\x82\x93\xE3\x81\xAB\xE3\x81\xA1wa\xE3\x80\x81"
		"\xE4\xB8\x96\xE7\x95\x8C\xEF\xBC\x81\x20\xCE\xB1\xCE\xB2\xCE\xB3\xCE\xB4", QrCode::QUARTILE);
	printQrCGA(qr2, "Unicode text as UTF-8");
	
	// Moderately large QR Code using longer text (from Lewis Carroll's Alice in Wonderland)
	const QrCode qr3 = QrCode::encodeText(
		"Alice was beginning to get very tired of sitting by her sister on the bank, "
		"and of having nothing to do: once or twice she had peeped into the book her sister was reading, "
		"but it had no pictures or conversations in it, 'and what is the use of a book,' thought Alice "
		"'without pictures or conversations?' So she was considering in her own mind (as well as she could, "
		"for the hot day made her feel very sleepy and stupid), whether the pleasure of making a "
		"daisy-chain would be worth the trouble of getting up and picking the daisies, when suddenly "
		"a White Rabbit with pink eyes ran close by her.", QrCode::HIGH);
	printQrCGA(qr3, "Moderately large QR Code using longer text (from Lewis Carroll's Alice in Wonderland)");
}


// Creates QR Codes with manually specified segments for better compactness.
static void doSegmentDemo() {
	// Illustration "silver"
	const char *silver0 = "THE SQUARE ROOT OF 2 IS 1.";
	const char *silver1 = "41421356237309504880168872420969807856967187537694807317667973799";
	const QrCode qr0 = QrCode::encodeText(
		(std::string(silver0) + silver1).c_str(),
		QrCode::LOW);
	
	printQrCGA(qr0, "Illustration \"silver\"");
	
	std::vector<QrSegment> segments;
	segments.push_back(QrSegment::makeAlphanumeric(silver0));
	segments.push_back(QrSegment::makeNumeric(silver1));
	
	const QrCode qr1 = QrCode::encodeSegments(segments, QrCode::LOW);
	printQrCGA(qr1, "Illustration \"silver\" alpha + numeric");
	
	// Illustration "golden"
	const char *golden0 = "Golden ratio \xCF\x86 = 1.";
	const char *golden1 = "6180339887498948482045868343656381177203091798057628621354486227052604628189024497072072041893911374";
	const char *golden2 = "......";
	const QrCode qr2 = QrCode::encodeText((std::string(golden0) + golden1 + golden2).c_str(), QrCode::LOW);
	printQrCGA(qr2, "Illustration \"golden\"");
	
	std::vector<uint8_t> bytes(golden0, golden0 + std::strlen(golden0));
	std::vector<QrSegment> segments3;
	segments3.push_back(QrSegment::makeBytes(bytes));
	segments3.push_back(QrSegment::makeNumeric(golden1));
	segments3.push_back(QrSegment::makeAlphanumeric(golden2));
	
	const QrCode qr3 = QrCode::encodeSegments(segments3, QrCode::LOW);
	printQrCGA(qr3, "Illustration \"golden\" bytes + numeric + alpha");
	
	// Illustration "Madoka": kanji, kana, Cyrillic, full-width Latin, Greek characters
	const char *madoka =  // Encoded in UTF-8
		"\xE3\x80\x8C\xE9\xAD\x94\xE6\xB3\x95\xE5"
		"\xB0\x91\xE5\xA5\xB3\xE3\x81\xBE\xE3\x81"
		"\xA9\xE3\x81\x8B\xE2\x98\x86\xE3\x83\x9E"
		"\xE3\x82\xAE\xE3\x82\xAB\xE3\x80\x8D\xE3"
		"\x81\xA3\xE3\x81\xA6\xE3\x80\x81\xE3\x80"
		"\x80\xD0\x98\xD0\x90\xD0\x98\xE3\x80\x80"
		"\xEF\xBD\x84\xEF\xBD\x85\xEF\xBD\x93\xEF"
		"\xBD\x95\xE3\x80\x80\xCE\xBA\xCE\xB1\xEF"
		"\xBC\x9F";
	const QrCode qr4 = QrCode::encodeText(madoka, QrCode::LOW);
	printQrCGA(qr4, "Illustration \"Madoka\": kanji, kana, Cyrillic, full-width Latin, Greek characters");
	
	std::vector<int> kanjiChars;  // Kanji mode encoding (13 bits per character)
	kanjiChars.push_back(0x0035);
	kanjiChars.push_back(0x1002);
	kanjiChars.push_back(0x0FC0);
	kanjiChars.push_back(0x0AED);
	kanjiChars.push_back(0x0AD7);
	kanjiChars.push_back(0x015C);
	kanjiChars.push_back(0x0147);
	kanjiChars.push_back(0x0129);
	kanjiChars.push_back(0x0059);
	kanjiChars.push_back(0x01BD);
	kanjiChars.push_back(0x018D);
	kanjiChars.push_back(0x018A);
	kanjiChars.push_back(0x0036);
	kanjiChars.push_back(0x0141);
	kanjiChars.push_back(0x0144);
	kanjiChars.push_back(0x0001);
	kanjiChars.push_back(0x0000);
	kanjiChars.push_back(0x0249);
	kanjiChars.push_back(0x0240);
	kanjiChars.push_back(0x0249);
	kanjiChars.push_back(0x0000);
	kanjiChars.push_back(0x0104);
	kanjiChars.push_back(0x0105);
	kanjiChars.push_back(0x0113);
	kanjiChars.push_back(0x0115);
	kanjiChars.push_back(0x0000);
	kanjiChars.push_back(0x0208);
	kanjiChars.push_back(0x01FF);
	kanjiChars.push_back(0x0008);

	qrcodegen::BitBuffer bb;

	for (std::size_t i = 0; i < kanjiChars.size(); i++)
		bb.appendBits(static_cast<uint32_t>(kanjiChars[i]), 13);

	QrSegment kanjiSegment(QrSegment::Mode::KANJI, static_cast<int>(kanjiChars.size()), bb);
	std::vector<QrSegment> kanjiSegments;
	kanjiSegments.push_back(kanjiSegment);
	
	const QrCode qr5 = QrCode::encodeSegments(kanjiSegments, QrCode::LOW);
	printQrCGA(qr5, "Kanji mode encoding (13 bits per character)");
}


// Creates QR Codes with the same size and contents but different mask patterns.
static void doMaskDemo() {
	// Project Nayuki URL
	std::vector<QrSegment> segs0 = QrSegment::makeSegments("https://www.nayuki.io/");
	printQrCGA(QrCode::encodeSegments(segs0, QrCode::HIGH, QrCode::MIN_VERSION, QrCode::MAX_VERSION, -1, true), "Automatic mask");
	printQrCGA(QrCode::encodeSegments(segs0, QrCode::HIGH, QrCode::MIN_VERSION, QrCode::MAX_VERSION, 3, true), "Force mask 3");
	
	// Chinese text as UTF-8
	std::vector<QrSegment> segs1 = QrSegment::makeSegments(
		"\xE7\xB6\xAD\xE5\x9F\xBA\xE7\x99\xBE\xE7\xA7\x91\xEF\xBC\x88\x57\x69\x6B\x69\x70"
		"\x65\x64\x69\x61\xEF\xBC\x8C\xE8\x81\x86\xE8\x81\xBD\x69\x2F\xCB\x8C\x77\xC9\xAA"
		"\x6B\xE1\xB5\xBB\xCB\x88\x70\x69\xCB\x90\x64\x69\x2E\xC9\x99\x2F\xEF\xBC\x89\xE6"
		"\x98\xAF\xE4\xB8\x80\xE5\x80\x8B\xE8\x87\xAA\xE7\x94\xB1\xE5\x85\xA7\xE5\xAE\xB9"
		"\xE3\x80\x81\xE5\x85\xAC\xE9\x96\x8B\xE7\xB7\xA8\xE8\xBC\xAF\xE4\xB8\x94\xE5\xA4"
		"\x9A\xE8\xAA\x9E\xE8\xA8\x80\xE7\x9A\x84\xE7\xB6\xB2\xE8\xB7\xAF\xE7\x99\xBE\xE7"
		"\xA7\x91\xE5\x85\xA8\xE6\x9B\xB8\xE5\x8D\x94\xE4\xBD\x9C\xE8\xA8\x88\xE7\x95\xAB");
	printQrCGA(QrCode::encodeSegments(segs1, QrCode::MEDIUM, QrCode::MIN_VERSION, QrCode::MAX_VERSION, 0, true), "Force mask 0");
	printQrCGA(QrCode::encodeSegments(segs1, QrCode::MEDIUM, QrCode::MIN_VERSION, QrCode::MAX_VERSION, 1, true), "Force mask 1");
	printQrCGA(QrCode::encodeSegments(segs1, QrCode::MEDIUM, QrCode::MIN_VERSION, QrCode::MAX_VERSION, 5, true), "Force mask 5");
	printQrCGA(QrCode::encodeSegments(segs1, QrCode::MEDIUM, QrCode::MIN_VERSION, QrCode::MAX_VERSION, 7, true), "Force mask 7");
}



/*---- Utilities ----*/

// Returns a string of SVG code for an image depicting the given QR Code, with the given number
// of border modules. The string always uses Unix newlines (\n), regardless of the platform.
static std::string toSvgString(const QrCode &qr, int border) {
	if (border < 0)
		throw std::domain_error("Border must be non-negative");
	if (border > INT_MAX / 2 || border * 2 > INT_MAX - qr.getSize())
		throw std::overflow_error("Border too large");
	
	std::string sb;
	sb += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	sb += "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
	sb += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" viewBox=\"0 0 ";

	// Convert numbers to strings using sprintf
	char buffer[32];
	std::sprintf(buffer, "%d", qr.getSize() + border * 2);
	sb += buffer;
	sb += " ";
	std::sprintf(buffer, "%d", qr.getSize() + border * 2);
	sb += buffer;
	sb += "\" stroke=\"none\">\n";
	sb += "\t<rect width=\"100%\" height=\"100%\" fill=\"#FFFFFF\"/>\n";
	sb += "\t<path d=\"";

	for (int y = 0; y < qr.getSize(); y++) {
		for (int x = 0; x < qr.getSize(); x++) {
			if (qr.getModule(x, y)) {
				sb += " M";
				std::sprintf(buffer, "%d", x + border);
				sb += buffer;
				sb += ",";
				std::sprintf(buffer, "%d", y + border);
				sb += buffer;
				sb += "h1v1h-1z";
			}
		}
	}	
	sb += "\" fill=\"#000000\"/>\n";
	sb += "</svg>\n";
	return sb;
}

// Prints the given QrCode object to the console.
static void printQr(const QrCode &qr) {
	int border = 4;
	for (int y = -border; y < qr.getSize() + border; y++) {
		for (int x = -border; x < qr.getSize() + border; x++) {
			std::cout << (qr.getModule(x, y) ? "\xDB\xDB" : "  ");
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

/*---- CGA Mode Helper ----*/

static void setVideoMode(uint8_t mode) {
	union REGS regs;
	regs.h.ah = 0x00;
	regs.h.al = mode;
	int86(0x10, &regs, &regs);
}

static uint8_t getVideoMode() {
	union REGS regs;
	regs.h.ah = 0x0F;
	int86(0x10, &regs, &regs);
	return regs.h.al;
}

// Function to set a pixel in CGA 320x200 4 color mode
static void setPixel(int x, int y, int color) {
	if (x < 0 || x >= 320 || y < 0 || y >= 200) {
	return;  // Ignore out-of-bounds pixels
	}
	
	uint8_t far* video = (uint8_t far*)0xB8000000L;
    
	// Calculate byte offset
	uint16_t offset = ((y & 1) << 13) + (y >> 1) * 80 + (x >> 2);
	uint8_t shift = (3 - (x & 3)) * 2; // shift amount for pixel within the byte
	
	uint8_t mask = 0x03 << shift;      // mask to clear pixel
	video[offset] = (video[offset] & ~mask) | ((color & 0x03) << shift);
}

// Function to render a centered QR code in CGA 320x200 mode
static void printQrCGA(const QrCode &qr, const char *info) {
	const int border = 4;  // Border size in QR modules
	// scale set to 1 for the variety demo, 4 looks best
	const int scaleX = 1;  // Horizontal scale factor (pixels per module)
	const int scaleY = 1;  // Vertical scale factor (pixels per module)

	// Calculate the total QR code size in pixels
	int qrSize = qr.getSize() + 2 * border;  // QR code size including border (in modules)
	int pixelWidth = qrSize * scaleX;       // Total width in pixels
	int pixelHeight = qrSize * scaleY;      // Total height in pixels

	// Ensure the QR code fits within the 320x200 resolution
	if (pixelWidth > 320 || pixelHeight > 200) {
		std::cerr << "Error: QR code is too large to fit on the screen." << std::endl;
		return;
	}

	// Calculate the starting position to center the QR code
	int startX = (320 - pixelWidth) / 2;  // Horizontal centering
	int startY = (200 - pixelHeight) / 2; // Vertical centering

	// remember current mode and set CGA 320x200 4 color mode
	uint8_t initialVideoMode = getVideoMode();
	setVideoMode(0x04);

	std::cout << info << std::endl;

	// Render the QR code
	for (int y = -border; y < qr.getSize() + border; y++) {  // rows
		for (int x = -border; x < qr.getSize() + border; x++) {  // columns
			int color = qr.getModule(x, y) ? 0 : 1;  // Black or white module
			for (int dy = 0; dy < scaleY; dy++) {  // vertical scaling
				for (int dx = 0; dx < scaleX; dx++) {  // horizontal scaling
					int xPos = startX + (x + border) * scaleX + dx;  // Horizontal position
					int yPos = startY + (y + border) * scaleY + dy;  // Vertical position
					setPixel(xPos, yPos, color);
				}
			}
		}
	}
	
	// Wait for a key press
	getch();
	
	// Restore previous mode
	setVideoMode(initialVideoMode);
}
