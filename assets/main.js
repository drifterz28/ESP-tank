const url = '192.168.0.109'; // for local testing
//const url = location.hostname;
const conn = new WebSocket(`ws://${url}:81/`, ['arduino']);

const utils = {};

const App = React.createClass({
  getInitialState() {
    return {
      isConnected: false,
      leftTrack: 0,
      rightTrack: 0
    };
  },
  handleError(err) {
    console.log('Error ', error);
    this.setState({isConnected: false});
  },
  componentWillMount() {
    // conn.onopen = () => {
    //   conn.send('Connect ' + new Date());
    // };
    // conn.onclose = (close) => {
    //   console.log(close);
    // };
    // conn.onerror = (error) => {
    //   this.handleError(error);
    // };
    // conn.onmessage = (e) => {
    //   this.handleUpdate(e.data);
    // };
  },
  componentWillUpdate(nextProps, nextState) {
    delete nextState.isConnected;
    console.log(JSON.stringify(nextState));
    //conn.send(JSON.stringify(nextState));
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
  render() {
    const {leftTrack, rightTrack} = this.state;
    return (
      <div className="app">
        <div className="tracks">
          {leftTrack}<br/>
          <input type="range" min="-255" max="255" name="left" value={leftTrack} name="leftTrack" onChange={this.handleChange}/>
        </div>
        <div className="tracks">
          {rightTrack}<br/>
          <input type="range" min="-255" max="255" name="left" value={rightTrack} name="rightTrack" onChange={this.handleChange}/>
        </div>
      </div>
    );
  }
});

ReactDOM.render(
  <App/>,
  document.querySelector('.container')
);
