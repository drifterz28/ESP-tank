//const url = '192.168.0.109'; // for local testing
const url = location.hostname;
let conn;

const utils = {};

const App = React.createClass({
  getInitialState() {
    return {
      isConnected: false,
      leftTrack: 0,
      rightTrack: 0
    };
  },
  handleError(error) {
    console.log('Error ', error);
    this.setState({isConnected: false});
  },
  componentWillMount() {
    try {
      const conn = new WebSocket(`ws://${url}:81/`, ['arduino']);
      conn.onopen = () => {
        conn.send('Connect ' + new Date());
      };
      conn.onclose = (close) => {
        console.log(close);
      };
      conn.onerror = (error) => {
        this.handleError(error);
      };
      conn.onmessage = (e) => {
        this.handleUpdate(e.data);
      };
    } catch(e) {
      this.handleError(e);
    }
  },
  componentWillUpdate(nextProps, nextState) {
    delete nextState.isConnected;
    console.log(JSON.stringify(nextState));
    if(this.state.isConnected) {
      conn.send(JSON.stringify(nextState));
    }
  },
  handleUpdate(data) {
    if(data === 'Connected') {
      this.setState({isConnected: true});
    } else {
      const json = JSON.parse(data);
      this.setState(json);
    }
  },
  handleChange(e) {
    const speed = e.target.value;
    const track = e.target.name;
    let state = {};
    state[track] = speed;
    this.setState(state);
  },
  changeToZero(e) {
    const track = e.target.name;
    let state = {};
    state[track] = 0;
    this.setState(state);
  },
  render() {
    const {leftTrack, rightTrack, isConnected} = this.state;
    return (
      <div className="app">
        <div className="tracks">
          {Math.floor(leftTrack / 1024 * 100)}%<br/>
          <input
            type="range"
            min="-1024"
            max="1024"
            name="left"
            value={leftTrack}
            name="leftTrack"
            onChange={this.handleChange}
            onTouchEnd={this.changeToZero}
            onMouseUp={this.changeToZero}
          />
        </div>
        <div className="tracks">
          {Math.floor(rightTrack / 1024 * 100)}%<br/>
          <input
            type="range"
            min="-1024"
            max="1024"
            name="left"
            value={rightTrack}
            name="rightTrack"
            onChange={this.handleChange}
            onMouseUp={this.changeToZero}
            onTouchEnd={this.changeToZero}
          />
        </div>
        {!isConnected && <div className="notConnected">Not Connected, but feel free to play with the controls</div>}
      </div>
    );
  }
});

ReactDOM.render(
  <App/>,
  document.querySelector('.container')
);
