import dayjs from "dayjs";
import duration from "dayjs/plugin/duration";

dayjs.extend(duration);

const dayjsInstance = dayjs;

export { dayjsInstance as dayjs };
