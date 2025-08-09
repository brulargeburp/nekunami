
import kivy
from kivy.app import App
from kivy.uix.boxlayout import BoxLayout
from kivy.lang import Builder
from kivy.properties import BooleanProperty, StringProperty
from kivy.clock import Clock
from kivy.utils import get_color_from_hex
import random

# Kivy version check
kivy.require('2.0.0')

# --- UI Layout and Styling (Kv Language) ---
# This is the Kivy equivalent of the HTML structure and CSS styling.
# It defines the widgets, their arrangement, and their appearance.
KV_STRING = """
<CircuitBreakerLayout>:
    orientation: 'vertical'
    padding: 40
    spacing: 30
    canvas.before:
        Color:
            rgba: root.colors.bg
        Rectangle:
            pos: self.pos
            size: self.size

    Label:
        text: 'IoT Breaker'
        font_name: 'Orbitron'
        font_size: '32sp'
        color: root.colors.primary
        size_hint_y: None
        height: self.texture_size[1]

    Label:
        text: 'STATUS: ' + ('ON' if root.is_on else 'OFF')
        font_name: 'Orbitron'
        font_size: '24sp'
        color: root.colors.on if root.is_on else root.colors.off
        size_hint_y: None
        height: self.texture_size[1]

    BoxLayout:
        orientation: 'horizontal'
        spacing: 20
        size_hint_y: None
        height: '50dp'

        Button:
            text: 'ON'
            font_name: 'Orbitron'
            font_size: '20sp'
            background_color: root.colors.on
            background_normal: '' # Needed to show background_color
            on_press: root.set_on()

        Button:
            text: 'OFF'
            font_name: 'Orbitron'
            font_size: '20sp'
            background_color: root.colors.off
            background_normal: '' # Needed to show background_color
            on_press: root.set_off()

    GridLayout:
        cols: 2
        spacing: 20

        BoxLayout:
            orientation: 'vertical'
            padding: 20
            canvas.before:
                Color:
                    rgba: root.colors.dark_bg
                Rectangle:
                    pos: self.pos
                    size: self.size
            Label:
                text: 'System Voltage'
                font_size: '14sp'
                size_hint_y: None
                height: self.texture_size[1]
            Label:
                text: root.system_voltage_text
                font_name: 'Orbitron'
                font_size: '36sp'
                color: root.colors.primary

        BoxLayout:
            orientation: 'vertical'
            padding: 20
            canvas.before:
                Color:
                    rgba: root.colors.dark_bg
                Rectangle:
                    pos: self.pos
                    size: self.size
            Label:
                text: 'Load Voltage'
                font_size: '14sp'
                size_hint_y: None
                height: self.texture_size[1]
            Label:
                text: root.load_voltage_text
                font_name: 'Orbitron'
                font_size: '36sp'
                color: root.colors.primary
"""

# Load the KV string
Builder.load_string(KV_STRING)

# --- Main Widget Logic ---
class CircuitBreakerLayout(BoxLayout):
    """
    This class is the root widget of our application. It contains the
    logic for handling state changes and updating the UI.
    """
    # Kivy properties that automatically update the UI when changed
    is_on = BooleanProperty(False)
    system_voltage_text = StringProperty('0.00 V')
    load_voltage_text = StringProperty('0.00 V')

    # Color palette, equivalent to CSS variables
    class Colors:
        bg = get_color_from_hex('#1a1a1a')
        dark_bg = get_color_from_hex('#282828')
        primary = get_color_from_hex('#00ff7f')
        off = get_color_from_hex('#ff4141')
        on = get_color_from_hex('#00ff7f')
        text = get_color_from_hex('#f0f0f0')

    colors = Colors()

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        # Start the voltage update loop
        Clock.schedule_interval(self.update_voltages, 1.0)

    def set_on(self):
        """Called when the ON button is pressed."""
        self.is_on = True

    def set_off(self):
        """Called when the OFF button is pressed."""
        self.is_on = False

    def update_voltages(self, dt):
        """
        This method is called periodically by the Clock.
        It simulates voltage readings and updates the labels.
        """
        if self.is_on:
            system_voltage = 12.0 + (random.random() * 0.5 - 0.25)
            load_voltage = 11.8 + (random.random() * 0.5 - 0.25)
            self.system_voltage_text = f'{system_voltage:.2f} V'
            self.load_voltage_text = f'{load_voltage:.2f} V'
        else:
            self.system_voltage_text = '0.00 V'
            self.load_voltage_text = '0.00 V'


# --- App Definition ---
class CircuitBreakerApp(App):
    """
    This is the main application class.
    """
    def build(self):
        """
        This method returns the root widget of the application.
        
        NOTE: For the 'Orbitron' font to work, you must:
        1. Download the font file (e.g., Orbitron-Regular.ttf).
        2. Place it in the same directory as this script, or a known font path.
        Kivy will automatically look for it by the name used in the kv lang string.
        """
        return CircuitBreakerLayout()


# --- Run the Application ---
if __name__ == '__main__':
    CircuitBreakerApp().run()
