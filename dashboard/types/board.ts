export type OnlineStatus = 0 | 1;

export type Leds = {
  color: string;
  brightness: number;
  status: OnlineStatus;
};

export type Board = {
  id: string;
  name: string;
  status: OnlineStatus;
  leds: Leds;
};
