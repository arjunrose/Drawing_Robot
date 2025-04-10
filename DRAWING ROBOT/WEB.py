from flask import Flask, request, jsonify, render_template, send_from_directory
import cv2
import numpy as np
import os
from datetime import datetime

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = 'uploads'
app.config['MAX_CONTENT_LENGTH'] = 16 * 1024 * 1024  # 16MB
os.makedirs(app.config['UPLOAD_FOLDER'], exist_ok=True)

# Store the last generated G-code
last_gcode = {
    'code': None,
    'image': None,
    'time': None
}

@app.route('/')
def index():
    app.logger.info("Rendering index page")
    return render_template('index.html', 
                         gcode=last_gcode['code'],
                         image=last_gcode['image'])

@app.route('/upload', methods=['POST'])
def upload_image():
    global last_gcode
    if 'image' not in request.files:
        app.logger.error("No file provided")
        return jsonify({'error': 'No file provided'}), 400
    file = request.files['image']
    if file.filename == '':
        app.logger.error("Empty filename")
        return jsonify({'error': 'Empty filename'}), 400
    try:
        # Save uploaded image with timestamp
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"{timestamp}_{file.filename}"
        filepath = os.path.join(app.config['UPLOAD_FOLDER'], filename)
        file.save(filepath)
        app.logger.info(f"File saved: {filepath}")

        # Process image
        image = cv2.imread(filepath, cv2.IMREAD_GRAYSCALE)
        _, binary_image = cv2.threshold(image, 127, 255, cv2.THRESH_BINARY_INV)
        # Generate G-code
        gcode_lines = ["G90", "G21", "M05 S90"]  # Initialize with pen up
        contours, _ = cv2.findContours(binary_image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        for contour in contours:
            epsilon = 0.01 * cv2.arcLength(contour, True)
            approx = cv2.approxPolyDP(contour, epsilon, True)
            gcode_lines.append("M05 S90")  # Pen up
            x, y = approx[0][0]
            gcode_lines.append(f"G0 X{x} Y{y}")
            gcode_lines.append("M03 S0")  # Pen down
            for point in approx[1:]:
                x, y = point[0]
                gcode_lines.append(f"G1 X{x} Y{y}")
        gcode_lines.append("M05 S90")  # Final pen up
        gcode = "\n".join(gcode_lines)  # Fixed line breaks
        # Store for display
        last_gcode = {
            'code': gcode,
            'image': filename,
            'time': timestamp
        }

        app.logger.info("G-code generated successfully")
        return jsonify({
            "gcode": gcode,
            "image_url": f"/uploads/{filename}"
        })
    except Exception as e:
        app.logger.error(f"Error processing image: {str(e)}")
        return jsonify({"error": str(e)}), 500

@app.route('/uploads/<filename>')
def uploaded_file(filename):
    app.logger.info(f"Serving uploaded file: {filename}")
    return send_from_directory(app.config['UPLOAD_FOLDER'], filename)

if __name__ == '__main__':
    app.logger.info("Starting Flask server")
    app.run(host='0.0.0.0', port=5000, debug=True)