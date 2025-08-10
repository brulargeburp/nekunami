        class Dashboard {
            constructor(container, isOverall = false) {
                this.container = container;
                this.isOverall = isOverall;
                this.statusText = container.querySelector('.status-text');
                this.btnOn = container.querySelector('.btn-on');
                this.btnOff = container.querySelector('.btn-off');
                this.systemVoltageDisplay = container.querySelector('.system-voltage');
                this.loadVoltageDisplay = container.querySelector('.load-voltage');

                this.isOn = false;
                this.voltageInterval = null;

                this.btnOn.addEventListener('click', () => this.setOn());
                this.btnOff.addEventListener('click', () => this.setOff());

                this.updateStatus();
            }

            updateStatus() {
                if (this.isOn) {
                    this.statusText.textContent = 'ON';
                    this.statusText.style.color = 'var(--on-color)';
                    this.startVoltageReadings();
                } else {
                    this.statusText.textContent = 'OFF';
                    this.statusText.style.color = 'var(--off-color)';
                    this.stopVoltageReadings();
                }
            }

            setOn() {
                if (this.isOverall || dashboards[0].isOn) {
                    this.isOn = true;
                    this.updateStatus();
                    if (this.isOverall) {
                        updateOverallLoad();
                    }
                }
            }

            setOff() {
                this.isOn = false;
                this.updateStatus();
                if (this.isOverall) {
                    dashboards.slice(1).forEach(d => d.setOff());
                }
                updateOverallLoad();
            }



            startVoltageReadings() {
                if (this.voltageInterval) clearInterval(this.voltageInterval);
                this.voltageInterval = setInterval(() => {
                    const systemVoltage = 12.0 + (Math.random() * 0.5 - 0.25);
                    const loadVoltage = this.isOn ? 11.8 + (Math.random() * 0.5 - 0.25) : 0;
                    
                    if (this.isOverall) {
                        this.systemVoltageDisplay.textContent = systemVoltage.toFixed(2) + ' V';
                    } else {
                        this.systemVoltageDisplay.textContent = dashboards[0].isOn ? systemVoltage.toFixed(2) + ' V' : '0.00 V';
                        this.loadVoltageDisplay.textContent = this.isOn ? loadVoltage.toFixed(2) + ' V' : '0.00 V';
                    }
                    if(this.isOn && !this.isOverall) updateOverallLoad();

                }, 1000);
            }

            stopVoltageReadings() {
                if (this.voltageInterval) clearInterval(this.voltageInterval);
                this.voltageInterval = null;
                if (!this.isOverall) {
                    this.systemVoltageDisplay.textContent = '0.00 V';
                }
                this.loadVoltageDisplay.textContent = '0.00 V';
            }
        }

        const dashboardElements = document.querySelectorAll('.container');
        const dashboards = Array.from(dashboardElements).map((el, i) => new Dashboard(el, i === 0));

        function updateOverallLoad() {
            let totalLoad = 0;
            dashboards.slice(1).forEach(d => {
                if (d.isOn) {
                    totalLoad += parseFloat(d.loadVoltageDisplay.textContent);
                }
            });
            dashboards[0].loadVoltageDisplay.textContent = totalLoad.toFixed(2) + ' V';
        }
        
        // Initial state
        dashboards[0].startVoltageReadings();