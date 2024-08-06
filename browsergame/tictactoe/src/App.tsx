import {useState, useEffect} from 'react';
import mqtt, {IClientOptions, MqttClient} from 'mqtt';
import {Box, Button, Table, TableBody, TableCell, TableRow, Typography} from '@mui/material';
import ClearIcon from '@mui/icons-material/Clear';
import CircleIcon from '@mui/icons-material/Circle';
import FilterListOffIcon from '@mui/icons-material/FilterListOff';

const subscribtions = ['connected', 'move', 'selectedPiece', 'startNumber'];

function App() {
	const [client, setClient] = useState<null | MqttClient>(null);
	const [connectStatus, setConnectStatus] = useState<string>('Not connected');
	const [isSub, setIsSub] = useState<boolean>(false);
	const [payload, setPayload] = useState<{topic: string; message: string}[]>([]);
	const [gameBoard, setGameBoard] = useState<number[][]>([
		[0, 0, 0],
		[0, 0, 0],
		[0, 0, 0],
	]);

	const mqttConnect = (host: string, mqttOption: IClientOptions) => {
		setConnectStatus('Connecting');
		setClient(mqtt.connect(host, mqttOption));
	};

	useEffect(() => {
		if (client) {
			console.log(client);
			client.on('connect', () => {
				setConnectStatus('Connected');
				subscribtions.forEach((topic) => {
					mqttSub({topic, qos: 0});
				});
			});
			client.on('error', (err) => {
				console.error('Connection error: ', err);
				client.end();
			});
			client.on('reconnect', () => {
				setConnectStatus('Reconnecting');
			});
			client.on('message', (topic, message) => {
				console.log('Message received', topic, message.toString());
				const payload = {topic, message: message.toString()};
				setPayload((prev) => [payload, ...prev]);
			});
		}
	}, [client]);

	const mqttSub = (subscription: {topic: string; qos: 0 | 1 | 2}) => {
		if (client) {
			const {topic, qos} = subscription;
			client.subscribe(topic, {qos}, (error) => {
				if (error) {
					console.log('Subscribe to topics error', error);
					return;
				}
				setIsSub(true);
			});
		}
	};

	const publish = (topic: string, message: string) => {
		if (client) {
			client.publish(topic, message);
		} else {
			console.log('Client not connected');
		}
	};

	const placePiece = (row: number, col: number, piece: number) => {
		const newBoard = gameBoard.map((r) => r.slice());
		newBoard[row][col] = piece;
		publish('move', JSON.stringify({row, col, MAC: 'LOL'}));
		setGameBoard(newBoard);
	};

	return (
		<Box sx={{px: 2}}>
			<Typography variant="h1">TicTacToe</Typography>
			<Typography variant="body1">
				Connect status: <Typography component="span">{connectStatus}</Typography>
			</Typography>
			{isSub && <Typography variant="body1">Subscribed to topics: {subscribtions.join(', ')}</Typography>}
			{!client?.connected && (
				<Button
					variant="contained"
					onClick={() => {
						if (client?.connected) {
							console.log('Already connected');
							return;
						}
						const url = process.env.REACT_APP_MQTT_URL;
						const username = process.env.REACT_APP_MQTT_USERNAME;
						const password = process.env.REACT_APP_MQTT_PASSWORD;
						if (!url || !username || !password) {
							console.error('Missing MQTT_URL, MQTT_USERNAME, or MQTT_PASSWORD');
							return;
						}
						console.log(url, username, password);
						mqttConnect(url, {
							username,
							password,
						});
					}}>
					Connect
				</Button>
			)}
			<Box sx={{mt: 2}}>
				<Typography variant="h2">Game</Typography>
				{
					<Table>
						<TableBody>
							{gameBoard.map((row, rowIndex) => (
								<TableRow key={rowIndex}>
									{row.map((cell, cellIndex) => (
										<TableCell
											key={`${rowIndex},${cellIndex}`}
											sx={{border: '2px solid black', width: '33%', height: '50px', textAlign: 'center', py: 1}}
											onClick={() => placePiece(rowIndex, cellIndex, 1)}>
											{cell === 0 ? '' : cell === 1 ? <ClearIcon sx={{fontSize: '42px'}} /> : <CircleIcon sx={{fontSize: '42px'}} />}
										</TableCell>
									))}
								</TableRow>
							))}
						</TableBody>
					</Table>
				}
			</Box>
			<Box sx={{display: 'flex', width: '100%', justifyContent: 'space-between', mt: 2}}>
				<Box>
					<Typography>Select Piece</Typography>
					<Button variant="contained" onClick={() => client?.publish('selectedPiece', JSON.stringify({selected: 0, MAC: 'LOL'}))}>
						<ClearIcon />
					</Button>
					<Button variant="contained" onClick={() => client?.publish('selectedPiece', JSON.stringify({selected: 1, MAC: 'LOL'}))}>
						<CircleIcon />
					</Button>
				</Box>
				<Box>
					<Typography>Payload</Typography>
					<Button variant="contained" onClick={() => setPayload([])}>
						<FilterListOffIcon />
					</Button>
					{payload.map((data, index) => (
						<Typography key={index}>
							[{data.topic}]: {data.message}
						</Typography>
					))}
				</Box>
				<Box>
					<Typography>Start number</Typography>
					<Button variant="contained" onClick={() => publish('startNumber', JSON.stringify({randLong: Math.floor(Math.random() * 1023), MAC: 'LOL'}))}>
						Generate number to see who starts
					</Button>
				</Box>
			</Box>
		</Box>
	);
}

export default App;
