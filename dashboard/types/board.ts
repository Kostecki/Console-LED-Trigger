export type OnlineStatus = 0 | 1;

export type Leds = {
  colorIndex: number;
  brightness: number;
  status: OnlineStatus;
};

export type Board = {
  id: string;
  status: OnlineStatus;
  name?: string;
  bootTime: number;
  leds: Leds;
};
