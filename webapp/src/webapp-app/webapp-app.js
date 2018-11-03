import { PolymerElement, html } from '@polymer/polymer/polymer-element.js';
import '@polymer/paper-button/paper-button.js';
import '@polymer/paper-styles/typography.js';
import '@polymer/paper-styles/typography.js';
import '@polymer/iron-ajax/iron-ajax.js';
import '@polymer/paper-card/paper-card.js';
import '@polymer/iron-pages/iron-pages.js';
import '@polymer/paper-input/paper-input.js';
import '@polymer/iron-flex-layout/iron-flex-layout-classes.js';
import '@polymer/paper-styles/color.js';
import '@polymer/paper-dialog/paper-dialog.js';
import '@polymer/paper-tabs/paper-tabs.js';
import '@polymer/paper-tabs/paper-tab.js';

/**
 * @customElement
 * @polymer
 */
class WebappApp extends PolymerElement {
  static get template() {
    return html`
    <style is="custom-style" include="iron-flex iron-flex-alignment">
      /* :host {
        transition: opacity 0.3s cubic-bezier(0.4, 0, 0.2, 1);
        --app-primary-color: var(--paper-indigo-500);
        --app-secondary-color: black;

      } */

      paper-tabs {
        background-color: var(--paper-blue-900);
        color: #fff;
      }

      paper-button{
        margin: 8px 8px 8px 8px
      }
      paper-button.blue {
        background: var(--paper-blue-700);
        color: white;
      }
      paper-button.green {
        background: var(--paper-green-700);
        color: white;
      }
      paper-button.red {
        background: var(--paper-red-700);
        color: white;
      }
      paper-button.orange {
        background: var(--paper-orange-700);
        color: white;
      }

      .content {
        @apply --layout-vertical;
        /* max-width: 500px; */
        padding: 16px;
        @apply --layout-center;     
      }

      .value {
        @apply --paper-font-subheading;
        font-weight: bold;
      }

      .label {
        @apply --paper-font-body2;
      }
      .subheading{
        @apply --paper-font-title;
        margin-bottom: 16px;
      }
    </style>
    <iron-ajax id="getDataAjax" auto="" url="[[_http_root]]all" handle-as="json" last-response="{{_data}}" debounce-duration="300" on-response="_onAllResponse">
    </iron-ajax>
    <iron-ajax id="getConfigAjax" auto="" url="[[_http_root]]config" handle-as="json" last-response="{{_config}}" debounce-duration="300" on-response="_onConfigResponse">
    </iron-ajax>
    <iron-ajax id="saveConfigAjax" method="POST" url="[[_http_root]]save-config" content-type="application/json" debounce-duration="300" on-response="_onSaveConfigResponse" on-error="_onSaveConfigError">
    </iron-ajax>
    
    <iron-ajax id="rebootAjax" method="GET" url="[[_http_root]]reboot" content-type="application/json" debounce-duration="300" on-response="_onSaveConfigResponse" on-error="_onSaveConfigError">
    </iron-ajax>

    <paper-dialog id="_dialog" modal>
      <h2>[[_dialogTitle]]</h2>
      <p>[[_dialogMessage]]</p>
      <div class="buttons">
        <paper-button dialog-confirm autofocus>OK</paper-button>
      </div>
    </paper-dialog>

    <div class="content">
      <iron-pages id="_pages" selected="0">
        <paper-card heading="Low Power Sensor">
          <div class="card-content">
              <div class="subheading">Current Information</div>
              <table fixed-column="">
              <tbody>
                <tr>
                  <td class="label">Temperature</td>
                  <td class="value">[[_data.temperature]]</td>
                </tr>
                <tr>
                  <td class="label">Voltage</td>
                  <td class="value">[[_data.voltage]]</td>
                </tr>
                <tr>
                  <td class="label">Free Heap</td>
                  <td class="value">[[_data.heap]]</td>
                </tr>
              </tbody>
            </table>
          </div>
          <div class="card-actions">
            <paper-button raised class="green" on-tap="_configure">Configure</paper-button>
          </div>
        </paper-card>

        <paper-card heading="Low Power Sensor">
          <div class="card-content">
            <div class="subheading">Configuration</div>
            <paper-tabs selected="{{_selectedConfTab}}">
              <paper-tab>Network</paper-tab>
              <paper-tab>InfluxDB</paper-tab>
            </paper-tabs>
            <iron-pages id="_ConfigPages" selected="{{_selectedConfTab}}">
              <div>
                  <paper-input always-float-label label="Hostname" value="{{_config.hostname}}"></paper-input>
                  <paper-input always-float-label label="WiFi AP Name" value="{{_config.wifi_ap}}"></paper-input>
                  <paper-input always-float-label label="WiFi Password" type="password" value="{{_config.wifi_password}}"></paper-input>
                  <paper-input always-float-label label="IP Address" value="{{_config.ipaddress}}"></paper-input>
                  <paper-input always-float-label label="Subnet Mask" value="{{_config.subnet}}"></paper-input>
                  <paper-input always-float-label label="DNS Server" value="{{_config.dnsserver}}"></paper-input>
                  <paper-input always-float-label label="Gateway" value="{{_config.gateway}}"></paper-input>
              </div>
              <div>
                <paper-input always-float-label label="Update Interval (Seconds)" value="{{_config.update_interval}}"></paper-input>
                <paper-input always-float-label label="Host" value="{{_config.influx_host}}"></paper-input>
                <paper-input always-float-label label="Port" value="{{_config.influx_port}}"></paper-input>
                <paper-input always-float-label label="Username" value="{{_config.influx_username}}"></paper-input>
                <paper-input always-float-label label="Password" type="password" value="{{_config.influx_password}}"></paper-input>
                <paper-input always-float-label label="Database" value="{{_config.influx_database}}"></paper-input>
                <paper-input always-float-label label="Line Temperature" value="{{_config.influx_line_temperature}}"></paper-input>
                <paper-input always-float-label label="Line Voltage" value="{{_config.influx_line_voltage}}"></paper-input>
              </div>

            </iron-pages>            
          </div>
          <div class="card-actions">
            <div>
            <paper-button raised class="orange" on-tap="_go_back">Back</paper-button>
            <paper-button raised class="red" on-tap="_onReboot">Reboot</paper-button>
            <paper-button raised class="green" on-tap="_save">Save</paper-button>
            </div>
          </div>
        </paper-card>

      </iron-pages>
    </div>

`;
  }

  static get is() { return 'webapp-app'; }

  static get observers() {
    return [
      '_pageChanged(_page)'
    ];
  }
  
  constructor() {
    super();
    this._data = null;
    this._actionParams = null;
    this._config = null;
    this._dialogTitle = null;
    this._dialogMessage = null;
    this._http_root = window.MyAppGlobals.rootPath;
    this._selectedConfTab = 0;
    this._page = 0;
  }

  _pageChanged(page){
    if (page == 1){ 
      this.$.getConfigAjax.generateRequest();
    }
    if(page == 0){
      this.$.getDataAjax.generateRequest();
    }
  }

  _configure() {
    this.$._pages.selected = 1;
  }

  _go_back() {
    this.$._pages.selected = 0;
  }

  _showMessage(title, message){
    this._dialogTitle = title;
    this._dialogMessage = message;
    this.$._dialog.open();
  }

  _onAllResponse(r) {
    console.log("response", r);
  }

  _onConfigResponse(r){
    console.log("response", r);
    if(this._config.wifi_ap == ''){
      this._configure();
    }
  }

  _save(){
    this.$.saveConfigAjax.body = this._config;
    console.log("Sending " + this.$.saveConfigAjax.body);
    this.$.saveConfigAjax.generateRequest();
  }

  _onSaveConfigResponse(event){
    console.log("Save Response", event);
    this._showMessage("Save Config",event.detail.statusText);
  }

  _onSaveConfigError(event, error){
    this._showMessage("Save Error",error.error);
  }

  _onReboot(){
    this.$.rebootAjax.generateRequest();
  }
}


window.customElements.define(WebappApp.is, WebappApp);
